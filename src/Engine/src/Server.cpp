/*
 *  This file is part of Goat Attack.
 *
 *  Goat Attack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Goat Attack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Goat Attack.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Server.hpp"
#include "Timing.hpp"
#include "Utils.hpp"
#include "ScopeAllocator.hpp"
#include "AutoPtr.hpp"

#include <string>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cerrno>

static const ns_t CycleS = 1000000000;

static const int CalcCyclesPerS = 60; /* 60Hz */
static const ns_t CalcCycleNS = CycleS / CalcCyclesPerS;

static const int BroadcastsPerS = 15;
static const int BroadcastCount = CalcCyclesPerS / BroadcastsPerS;

static const uint16_t MasterHeartbeatPort = 25112;

static const size_t MaxServerMsgLength = PacketMaxSize - 100;

/* ingame server constructor */
Server::Server(Resources& resources, Subsystem& subsystem, const KeyValue& kv, GamePlayType type,
    const std::string& map_name, int duration, int warmup) throw (Exception)
    : Properties(kv),
      ClientServer(subsystem.get_i18n(), atoi(get_value("port").c_str()), atoi(get_value("num_players").c_str()), get_value("server_name"), ""),
      ServerAdmin(resources, *this, *this),
      resources(resources), subsystem(subsystem),
      factory(resources, subsystem, 0),
      nbr_logout_msg(0), running(false), current_config(0), score_board_counter(0),
      warmup(false), hold_disconnected_players(atoi(get_value("hold_disconnected_player").c_str()) != 0 ? true : false),
      reconnect_kills(atoi(get_value("reconnect_kills").c_str())),
      hdp_counter(0), master_server(0), ms_counter(0), master_socket(),
      rotation_current_index(0),  log_file(0), logger(subsystem.get_stream(), true),
      reload_map_rotation(false), broadcast_settings(false)
{
    setup_loaded_paks();
    map_configs.push_back(MapConfiguration(type, map_name, duration, warmup));

    /* create server admin console */
    set_server(this);
    set_admin_server_is_on_client(true);
}

/* dedicated server constructor */
Server::Server(Resources& resources, Subsystem& subsystem,
    const std::string& server_config_file) throw (Exception)
    : Properties(server_config_file),
      ClientServer(subsystem.get_i18n(), atoi(get_value("port").c_str()), atoi(get_value("num_players").c_str()), get_value("server_name"), get_value("server_password")),
      ServerAdmin(resources, *this, *this),
      resources(resources), subsystem(subsystem),
      factory(resources, subsystem, 0),
      nbr_logout_msg(0), running(false), current_config(0), score_board_counter(0),
      warmup(false), hold_disconnected_players(atoi(get_value("hold_disconnected_player").c_str()) != 0 ? true : false),
      reconnect_kills(atoi(get_value("reconnect_kills").c_str())),
      hdp_counter(0), master_server(resolve_host(get_value("master_server"))),
      ms_counter(0), master_socket(), rotation_current_index(0),
      log_file(0), logger(create_log_stream(), true),
      reload_map_rotation(false), broadcast_settings(false)
{
    setup_loaded_paks();
    load_map_rotation();

    /* create server admin console */
    set_server(this);
}

Server::~Server() {
    /* delete complete client pak list */
    destroy_paks(0);

    /* free players */
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        delete *it;
    }

    cleanup_held_players();

    if (tournament) {
        delete tournament;
        tournament = 0;
    }

    if (log_file) {
        delete log_file;
    }
}

void Server::start() throw (ServerException) {
#ifdef DEDICATED_SERVER
    running = true;
    thread();
#else
    if (!running) {
        running = true;
        if (!thread_start()) {
            running = false;
            throw ServerException(ClientServer::i18n(I18N_THREAD_FAILED));
        }
    }
#endif
}

void Server::stop() {
    if (running) {
        running = false;
        thread_join();
    }
}

void Server::reload_config() throw (ServerException) {
    /* this is currently a dirty hack. in future i'll change passing the
     * Properties& through all levels and call an update() from derived class */
    hold_disconnected_players = (atoi(get_value("hold_disconnected_player").c_str()) != 0 ? true : false);
    reconnect_kills = atoi(get_value("reconnect_kills").c_str());
    if (!get_admin_server_is_on_client()) {
        master_server = resolve_host(get_value("master_server"));
        reload_map_rotation = true;
    }
    broadcast_settings = true;
    factory.set_tournament_server_flags(*this, tournament);
}

void Server::thread() {
    char msbuf[64];
    bool done;

    try {
        /* count logout messages */
        int curtxt = static_cast<int>(I18N_TNMT_LOGOUT_MSG01);
        while (i18n(static_cast<I18NText>(curtxt)).length()) {
            nbr_logout_msg++;
            curtxt++;
        }

        /* start */
        gametime_t now;
        gametime_t last;

        ns_t diff_now = 0;
        ms_t diff_milliseconds = 0;

        /* init, construct tournament in 20ms */
        get_now(last);
        add_ns(20000000, last);

        /* loop */
        int send_counter = 0;
        while (running) {
            try {
                /* process incoming messages */
                try {
                    while (cycle());
                } catch (const Exception& e) {
                    subsystem << e.what() << std::endl;
                }

                /* time over? */
                get_now(now);
                diff_now = diff_ns(last, now);
                if (diff_now >= CalcCycleNS) {
                    diff_milliseconds = diff_ms(last, now);
                    last = now;

                    /* tournament update */
                    if (tournament) {
                        tournament->update_states(diff_now);
                        if (broadcast_settings) {
                            broadcast_settings = false;
                            tournament->retrieve_flags();
                        }
                        Tournament::StateResponses& responses = tournament->get_state_responses();
                        size_t sz = responses.size();
                        for (size_t i = 0; i < sz; i++) {
                            StateResponse *resp = responses[i];
                            stacked_broadcast_data_synced(factory.get_tournament_id(), resp->action, NetFlagsReliable, resp->len, resp->data);
                        }
                        tournament->delete_responses();

                        /* respawn request of player? */
                        for (Players::iterator it = players.begin(); it != players.end(); it++) {
                            Player *p = *it;
                            if (p->client_synced) {
                                if (p->respawning) {
                                    p->respawning = false;
                                    if (!p->is_alive_and_playing()) {
                                        p->state.server_state.flags &= PlayerServerFlagTeamRed;
                                        p->zero();
                                        GPlayerState state;
                                        memset(&state, 0, GPlayerStateLen);
                                        state = p->state;
                                        state.to_net();
                                        stacked_broadcast_data_synced(factory.get_tournament_id(), GPCSpawnPlayer, NetFlagsReliable, GPlayerStateLen, &state);
                                    }
                                }
                            }
                        }

                        /* flush */
                        flush_stacked_broadcast_data_synced(NetFlagsReliable);
                    }

                    bool new_tournament = false;
                    if (!tournament) {
                        new_tournament = true;
                    } else {
                        if (!tournament->get_game_state().seconds_remaining) {
                            if (warmup) {
                                new_tournament = true;
                            } else {
                                /* count down score board */
                                score_board_counter -= diff_milliseconds;
                                if (score_board_counter <= 0) {
                                    score_board_counter = 0;
                                    new_tournament = true;
                                }
                            }
                        }
                    }

                    if (new_tournament) {
                        /* broadcast new tournament */
                        if (players.size()) {
                            bool switch_to_game = select_map();
                            /* send map */
                            GTournament gt;
                            memset(&gt, 0, GTournamentLen);
                            strncpy(gt.map_name, current_config->map_name.c_str(), NameLength - 1);
                            gt.duration = static_cast<scounter_t>(current_config->duration);
                            gt.warmup = static_cast<scounter_t>(current_config->warmup_in_seconds);
                            gt.gametype = static_cast<unsigned char>(current_config->type);
                            gt.tournament_id = factory.get_tournament_id();
                            gt.flags |= (warmup ? TournamentFlagWarmup : 0);
                            gt.to_net();
                            stacked_broadcast_data_synced(factory.get_tournament_id(), GPCMapState, NetFlagsReliable, GTournamentLen, &gt);

                            /* send tournament flags */
                            if (tournament) {
                                tournament->retrieve_flags();
                                Tournament::StateResponses& responses = tournament->get_state_responses();
                                size_t sz = responses.size();
                                for (size_t i = 0; i < sz; i++) {
                                    StateResponse *resp = responses[i];
                                    stacked_broadcast_data_synced(factory.get_tournament_id(), resp->action, NetFlagsReliable, resp->len, resp->data);
                                }
                                tournament->delete_responses();
                            }

                            /* send object states */
                            GPlaceObject gpo;
                            Tournament::GameObjects& objects = tournament->get_game_objects();
                            for (Tournament::GameObjects::iterator it = objects.begin();
                                it != objects.end(); it++)
                            {
                                GameObject *obj = *it;
                                if (!obj->picked) {
                                    gpo.id = obj->state.id;
                                    gpo.flags = 0;
                                    gpo.x = static_cast<pos_t>(obj->state.x);
                                    gpo.y = static_cast<pos_t>(obj->state.y);
                                    gpo.to_net();
                                    stacked_broadcast_data_synced(factory.get_tournament_id(), GPCPlaceObject, NetFlagsReliable, GPlaceObjectLen, &gpo);
                                }
                            }

                            /* reset all players */
                            for (Players::iterator it = players.begin();
                                it != players.end(); it++)
                            {
                                Player *p = *it;
                                if (p->client_synced) {
                                    if (switch_to_game) {
                                        p->clear();
                                        if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
                                            tournament->spawn_player(p);
                                            p->state.server_state.flags &= ~PlayerServerFlagDead;
                                        }
                                    } else {
                                        p->reset();
                                    }
                                }
                                GPlayerInfo info;
                                memset(&info, 0, sizeof(GPlayerInfo));

                                strncpy(info.desc.player_name, p->get_player_name().c_str(),
                                    NameLength - 1);

                                strncpy(info.desc.characterset_name, p->get_characterset_name().c_str(),
                                    NameLength - 1);

                                info.id = p->state.id;
                                info.server_state = p->state.server_state;
                                info.client_server_state = p->state.client_server_state;
                                info.client_state = p->state.client_state;
                                info.to_net();
                                stacked_broadcast_data_synced(factory.get_tournament_id(), GPCResetPlayer, NetFlagsReliable, GPlayerInfoLen, &info);
                            }

                            /* send generic states */
                            if (tournament) {
                                GenericData *gd = tournament->create_generic_data();
                                GenericData *pgd = gd;
                                while (pgd) {
                                    stacked_broadcast_data_synced(factory.get_tournament_id(), GPCGenericData, NetFlagsReliable, pgd->sz, pgd->data);
                                    pgd = pgd->next;
                                }
                                tournament->destroy_generic_data_list(gd);
                            }

                            /* send ready */
                            stacked_broadcast_data_synced(factory.get_tournament_id(), GPCReady, NetFlagsReliable, 0, 0);

                            /* send notification */
                            stacked_broadcast_data_synced(factory.get_tournament_id(), (warmup ? GPCWarmUp : GPCGameBegins), NetFlagsReliable, 0, 0);

                            /* server log */
                            if (warmup) {
                                logger.log(ServerLogger::LogTypeWarmUp, ClientServer::i18n(I18N_SERVE_WARM_UP));
                            } else {
                                logger.log(ServerLogger::LogTypeGameBegins, ClientServer::i18n(I18N_SERVE_GAME_BEGINS));
                            }

                            /* flush */
                            flush_stacked_broadcast_data_synced(NetFlagsReliable);
                        }
                    } else {
                        /* broadcast */
                        done = false;
                        send_counter++;
                        if (send_counter >= BroadcastCount) {
                            if (hold_disconnected_players) {
                                hdp_counter++;
                            }
                            send_counter = 0;
                            if (tournament) {
                                /* update game state */
                                GGameState gmstat = tournament->get_game_state();
                                gmstat.to_net();
                                stacked_broadcast_data_synced(factory.get_tournament_id(), GPCUpdateGameState, 0, GGameStateLen, &gmstat);

                                /* update player position and state */
                                done = true;
                                if (players.size()) {
                                    GPTAllStates stat;
                                    for (Players::iterator it = players.begin(); it != players.end(); it++) {
                                        Player *p = *it;
                                        if (p->client_synced) {
                                            const Connection *c = p->get_connection();
                                            if (c) {
                                                p->state.server_state.ping_time = static_cast<pico_size_t>(c->ping_time);
                                                size_t outq_sz = get_outq_sz(c);
                                                p->state.server_state.outq_sz = static_cast<nano_size_t>(outq_sz > 255 ? 255 : outq_sz);
                                            }
                                            stat.id = p->state.id;
                                            stat.server_state = p->state.server_state;
                                            stat.client_server_state = p->state.client_server_state;
                                            stat.to_net();
                                            stacked_broadcast_data_synced(factory.get_tournament_id(), GPCUpdatePlayerState, 0, GPTAllStatesLen, &stat);
                                        }
                                    }
                                }

                                /* update objects */
                                GObjectState objstat;
                                Tournament::GameObjects& objects = tournament->get_game_objects();
                                for (Tournament::GameObjects::iterator it = objects.begin();
                                    it != objects.end(); it++)
                                {
                                    GameObject *obj = *it;
                                    if (obj->object->get_physics()) {
                                        objstat = obj->state;
                                        objstat.to_net();
                                        stacked_broadcast_data_synced(factory.get_tournament_id(), GPCUpdateObject, 0, GObjectStateLen, &objstat);
                                    }
                                }

                                /* update animations */
                                GAnimationState anistat;
                                Tournament::GameAnimations& animations = tournament->get_game_animations();
                                for (Tournament::GameAnimations::iterator it = animations.begin();
                                    it != animations.end(); it++)
                                {
                                    GameAnimation *ani = *it;
                                    if (ani->animation->get_physics()) {
                                       anistat = ani->state;
                                       anistat.to_net();
                                       stacked_broadcast_data_synced(factory.get_tournament_id(), GPCUpdateAnimation, 0, GAnimationStateLen, &anistat);
                                    }
                                }

                                /* update spawnable npcs */
                                GNPCState npcstat;
                                Tournament::SpawnableNPCs& spawnable_npcs = tournament->get_spawnable_npcs();
                                for (Tournament::SpawnableNPCs::iterator it = spawnable_npcs.begin();
                                    it != spawnable_npcs.end(); it++)
                                {
                                    SpawnableNPC *npc = *it;
                                    npcstat = npc->state;
                                    npcstat.to_net();
                                    stacked_broadcast_data_synced(factory.get_tournament_id(), GPCUpdateSpawnableNPC, 0, GNPCStateLen, &npcstat);
                                }
                            }
                            /* flush */
                            flush_stacked_broadcast_data_synced(0);
                        }

                        /* special player broadcasts? */
                        if (!done) {
                            GPTAllStates stat;
                            bool force_broadcast = false;
                            for (Players::iterator it = players.begin(); it != players.end(); it++) {
                                Player *p = *it;
                                if (p->client_synced) {
                                    const Connection *c = p->get_connection();
                                    if (c) {
                                        p->state.server_state.ping_time = static_cast<pico_size_t>(c->ping_time);
                                        size_t outq_sz = get_outq_sz(c);
                                        p->state.server_state.outq_sz = static_cast<nano_size_t>(outq_sz > 255 ? 255 : outq_sz);
                                    }
                                    if (p->state.client_server_state.flags & PlayerClientServerFlagForceBroadcast) {
                                        p->state.client_server_state.flags &= ~PlayerClientServerFlagForceBroadcast;
                                        force_broadcast = true;
                                        stat.id = p->state.id;
                                        stat.server_state = p->state.server_state;
                                        stat.client_server_state = p->state.client_server_state;
                                        stat.to_net();
                                        stacked_broadcast_data_synced(factory.get_tournament_id(), GPCUpdatePlayerState, 0, GPTAllStatesLen, &stat);
                                    }
                                }
                            }
                            if (force_broadcast) {
                                flush_stacked_broadcast_data_synced(0);
                            }
                        }

                        /* delete old held player stats */
                        if (hdp_counter >= 60) {
                            hdp_counter = 0;
                            for (HeldPlayerStats::iterator it = held_player_stats.begin();
                                it != held_player_stats.end(); it++)
                            {
                                PlayerStats *ps = *it;
                                ms_t diff = diff_ms(ps->disconnect_time, now);
                                /* 60 seconds */
                                if (diff > 60000) {
                                    delete ps;
                                    held_player_stats.erase(it);
                                    break;
                                }
                            }
                        }
                    }
                }

                /* update master server all approx. 10 seconds */
                if (master_server) {
                    ms_counter++;
                    if (ms_counter >= 10000) {
                        ms_counter = 0;
                        sprintf(msbuf, "%hu", get_port());
                        master_socket.send(master_server, MasterHeartbeatPort, msbuf, strlen(msbuf));
                    }
                }

                /* delayed force quit clients? (once per cycle) */
                for (Players::iterator it = players.begin(); it != players.end(); it++) {
                    Player *p = *it;
                    if (p->server_force_quit_client) {
                        p->server_force_quit_client_counter--;
                        if (!p->server_force_quit_client_counter) {
                            kill(p->get_connection());
                            break;
                        }
                    }
                }

                /* wait for 1 ms */
                wait_ns(1000000);
            } catch (const Exception& e) {
                subsystem << e.what() << std::endl;
                broadcast_data(0, GPCServerQuit, NetFlagsReliable, strlen(e.what()), e.what());
            }
        }
    } catch (const Exception& e) {
        subsystem << e.what() << std::endl;
    }
}

void Server::event_login(const Connection *c, data_len_t len, void *data) throw (Exception) {
    GPlayerDescription *desc = reinterpret_cast<GPlayerDescription *>(data);
    desc->player_name[NameLength - 1] = 0;
    desc->characterset_name[NameLength - 1] = 0;
    size_t sz = players.size();

    /* brute force finding routine, have to improve it sometime */
    player_id_t player_id = 1;
    bool found;
    do {
        found = false;
        for (size_t i = 0; i < sz; i++) {
            Player *p = players[i];
            if (p->state.id == player_id) {
                found = true;
                break;
            }
        }
        if (found) {
            player_id++;
        }
    } while (found);

    /* add new player */
    Player *p = new Player(resources, c, player_id, desc->player_name, desc->characterset_name);
    players.push_back(p);
    sz++;

    /* reattach disconnect player? */
    found = false;
    if (hold_disconnected_players) {
        for (HeldPlayerStats::iterator it = held_player_stats.begin();
            it != held_player_stats.end(); it++)
        {
            PlayerStats *ps = *it;
            if (ps->host == c->host && ps->player_name == p->get_player_name()) {
                /* reconnect if same tournament */
                if (ps->tournament_id == factory.get_tournament_id()) {
                    found = true;
                    p->state.server_state = ps->server_state;
                    p->state.client_server_state = ps->client_server_state;
                    if (tournament && !tournament->is_game_over()) {
                        p->state.server_state.kills += reconnect_kills;
                        p->state.server_state.score -= reconnect_kills;
                    }
                    p->state.server_state.flags |= PlayerServerFlagDead;
                }
                delete ps;
                held_player_stats.erase(it);
                break;
            }
        }
    }

    I18NText conmsg = (found ? I18N_SERVE_RECONNECT : I18N_SERVE_CONNECT);

    /* console log */
    logger.log(ServerLogger::LogTypePlayerConnect, ClientServer::i18n(conmsg, p->get_player_name().c_str()) , p);

    /* send identifer to new client */
    player_id_t net_player_id = htons(player_id);
    send_data(c, factory.get_tournament_id(), GPCIdentifyPlayer, NetFlagsReliable, sizeof(net_player_id), &net_player_id);

    /* send all player states */
    GPlayerInfo info;
    for (size_t i = 0; i < sz; i++) {
        Player *p = players[i];
        memset(&info, 0, sizeof(GPlayerInfo));
        strncpy(info.desc.player_name, p->get_player_name().c_str(), NameLength - 1);
        strncpy(info.desc.characterset_name, p->get_characterset_name().c_str(), NameLength - 1);
        info.id = p->state.id;
        info.server_state = p->state.server_state;
        info.client_server_state = p->state.client_server_state;
        info.client_state = p->state.client_state;
        info.to_net();
        if (p->get_connection() == c) {
            broadcast_data(factory.get_tournament_id(), GPCAddPlayer, NetFlagsReliable, GPlayerInfoLen, &info);
        } else {
            send_data(c, factory.get_tournament_id(), GPCAddPlayer, NetFlagsReliable, GPlayerInfoLen, &info);
        }
    }

    /* send text message */
    size_t msglen;
    AutoPtr<char[]> txtptr(Tournament::create_i18n_response(conmsg, msglen, p->get_player_name().c_str()));
    GI18NText *txtmsg = reinterpret_cast<GI18NText *>(&txtptr[0]);
    broadcast_data(factory.get_tournament_id(), GPCI18NText, NetFlagsReliable, static_cast<data_len_t>(msglen), txtmsg);

    /* fill server pak list */
    PlayerClientPak *pcpak = new PlayerClientPak(p);
    player_client_paks.push_back(pcpak);
    for (Resources::LoadedPaks::const_iterator it = my_paks.begin(); it != my_paks.end(); it++) {
        const Resources::LoadedPak& my_pak = *it;
        pcpak->client_paks.push_back(ClientPak(&my_pak));
    }
}

void Server::event_data(const Connection *c, data_len_t len, void *data) throw (Exception) {
    /* find player */
    size_t sz = players.size();
    Player *p = 0;
    for (size_t i= 0; i < sz; i++) {
        Player *tp = players[i];
        if (tp->get_connection() == c) {
            p = tp;
            break;
        }
    }

    if (p) {
        GTransport *t = reinterpret_cast<GTransport *>(data);

        while (true) {
            t->from_net();

#ifdef SAFE_ALIGNMENT
            data_t *data_ptr = aligned_buffer;
            memcpy(data_ptr, t->data, t->len);
#else
            data_t *data_ptr = t->data;
#endif
            switch (t->cmd) {
                case GPSUpdatePlayerClientServerState:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            GPlayerClientServerState *state = reinterpret_cast<GPlayerClientServerState *>(data_ptr);
                            state->from_net();
                            p->state.client_server_state = *state;
                        }
                    }
                    break;
                }

                case GPSRespawnRequest:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            p->respawning = true;
                        }
                    }
                    break;
                }

                case GPSJoinRequest:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            playerflags_t *flags = reinterpret_cast<playerflags_t *>(data_ptr);
                            if (tournament) {
                                if (tournament->get_game_state().seconds_remaining) {
                                    if (!tournament->player_joins(p, *flags)) {
                                        send_data(c, factory.get_tournament_id(), GPCJoinRefused, NetFlagsReliable, 0, 0);
                                    } else {
                                        send_data(c, factory.get_tournament_id(), GPCJoinAccepted, NetFlagsReliable, 0, 0);
                                        p->respawning = true;
                                    }
                                } else {
                                    send_data(c, factory.get_tournament_id(), GPCJoinRefused, NetFlagsReliable, 0, 0);
                                }
                            }
                        }
                    }
                    break;
                }

                case GPSChatMessage:
                {
                    if (t->len && data_ptr[0] == '/') {
                        try {
                            parse_command(c, p, t->len, data_ptr);
                        } catch (const Exception& e) {
                            send_data(c, factory.get_tournament_id(), GPCTextMessage, NetFlagsReliable, strlen(e.what()), e.what());
                        }
                    } else {
                        AutoPtr<char[]> storage(new char[GChatMessageLen + t->len]);
                        GChatMessage *chat_msg = reinterpret_cast<GChatMessage *>(&storage[0]);
                        chat_msg->id = p->state.id;
                        chat_msg->len = t->len;
                        memcpy(chat_msg->data, data_ptr, t->len);
                        chat_msg->to_net();
                        broadcast_data(0, GPCChatMessage, NetFlagsReliable, GChatMessageLen + t->len, chat_msg);
                    }
                    break;
                }

                case GPSShot:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            if (tournament) {
                                unsigned char *dir = reinterpret_cast<unsigned char *>(data_ptr);
                                tournament->fire_shot(p, *dir);
                            }
                        }
                    }
                    break;
                }

                case GPSGrenade:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            if (tournament) {
                                unsigned char *dir = reinterpret_cast<unsigned char *>(data_ptr);
                                tournament->fire_grenade(p, *dir);
                            }
                        }
                    }
                    break;
                }

                case GPSBomb:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            if (tournament) {
                                unsigned char *dir = reinterpret_cast<unsigned char *>(data_ptr);
                                tournament->fire_bomb(p, *dir);
                            }
                        }
                    }
                    break;
                }

                case GPSFrog:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            if (tournament) {
                                unsigned char *dir = reinterpret_cast<unsigned char *>(data_ptr);
                                tournament->fire_frog(p, *dir);
                            }
                        }
                    }
                    break;
                }

                case GPSPlayerChanged:
                {
                    GPlayerDescription *pdesc = reinterpret_cast<GPlayerDescription *>(data_ptr);
                    std::string old_name = p->get_player_name();
                    std::string new_name(pdesc->player_name);
                    std::string new_skin(pdesc->characterset_name);
                    bool name_changed = (old_name != new_name);
                    if (name_changed) {
                        logger.log(ServerLogger::LogTypePlayerNameChange, ClientServer::i18n(I18N_CLIENT_RENAME, old_name, new_name), 0, 0, old_name.c_str(), new_name.c_str());
                    }
                    if (name_changed || p->get_characterset_name() != new_skin) {
                        p->set_player_name(new_name);
                        p->set_characterset(new_skin);
                        memset(pdesc->characterset_name, 0, sizeof(pdesc->characterset_name));
                        strcpy(pdesc->characterset_name, p->get_characterset_name().c_str());
                        pdesc->id = p->state.id;
                        pdesc->to_net();
                        broadcast_data(factory.get_tournament_id(), GPCPlayerChanged, NetFlagsReliable, t->len, data_ptr);
                    }
                    break;
                }

                case GPSRoundFinished:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        if (p->client_synced) {
                            if (tournament) {
                                GTransportTime *race = reinterpret_cast<GTransportTime *>(data_ptr);
                                race->from_net();
                                tournament->round_finished_set_time(p, race);
                            }
                        }
                    }
                    break;
                }

                case GPSPakSyncHash:
                {
                    GPakHash *hash = reinterpret_cast<GPakHash *>(data_ptr);
                    hash->from_net();
                    std::string pak_name = hash->pak_name;
                    std::string pak_hash(hash->pak_hash, GPakHash::HashLength);
                    for (Resources::LoadedPaks::const_iterator it = my_paks.begin(); it != my_paks.end(); it++) {
                        const Resources::LoadedPak& my_pak = *it;
                        if (my_pak.pak_short_name == pak_name) {
                            if (my_pak.pak_hash == pak_hash) {
                                /* hash is correct, remove from download list */
                                PlayerClientPak *pcpak = get_player_client_pak(p);
                                for (ClientPaks::iterator it = pcpak->client_paks.begin(); it != pcpak->client_paks.end(); it++) {
                                    ClientPak& cpak = *it;
                                    if (cpak.pak->pak_short_name == my_pak.pak_short_name) {
                                        pcpak->client_paks.erase(it);
                                        break;
                                    }
                                }
                            } else {
                                /* non downloadable pak file? */
                                const Resources::NonDownloadableMainPak *ptr = Resources::NonDownloadableMainPaks;
                                while (ptr->name) {
                                    if (!strcmp(ptr->name, pak_name.c_str())) {
                                        if (ptr->check) {
                                            quit_client(c, p, ClientServer::i18n(I18N_DIFFERENT_HASH, pak_name));
                                        }
                                        break;
                                    }
                                    ptr++;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }

                case GPSPakSyncHashFinished:
                case GPSPakSyncAck:
                {
                    try {
                        process_sync_pak(c, p);
                    } catch (const ServerException& e) {
                        quit_client(c, p, e.what());
                    }
                    break;
                }

                case GPSSpectate:
                {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        /* send disappear animation */
                        if (p->is_alive_and_playing()) {
                            GAnimation ani;
                            memset(&ani, 0, sizeof(GAnimation));
                            strncpy(ani.animation_name, "disappear", NameLength - 1);
                            ani.id = tournament->create_animation_id();
                            ani.x = p->state.client_server_state.x + Characterset::Colbox.x;
                            ani.y = p->state.client_server_state.y - (Characterset::Height / 2.0f);
                            ani.to_net();
                            stacked_broadcast_data_synced(factory.get_tournament_id(), GPCAddAnimation, NetFlagsReliable, GAnimationLen, &ani);
                        }

                        /* send information text */
                        size_t msglen;
                        AutoPtr<char[]> txtptr(Tournament::create_i18n_response(I18N_SERVE_SPECTATING, msglen, p->get_player_name().c_str()));
                        GI18NText *txtmsg = reinterpret_cast<GI18NText *>(&txtptr[0]);
                        stacked_broadcast_data_synced(0, GPCI18NText, NetFlagsReliable, static_cast<data_len_t>(msglen), txtmsg);

                        /* remove player from tournament and reset */
                        logger.log(ServerLogger::LogTypeLeft, ClientServer::i18n(I18N_SERVE_SPECTATING, p->get_player_name()), p);
                        tournament->player_removed(p);
                        tournament->player_added(p);
                        p->reset();
                        player_id_t id = htons(p->state.id);
                        stacked_broadcast_data_synced(factory.get_tournament_id(), GPCSpectate, NetFlagsReliable, sizeof(id), &id);

                        /* flush */
                        flush_stacked_broadcast_data_synced(NetFlagsReliable);
                    }
                    break;
                }
            }

            /* advance to next element */
            if (t->flags & TransportFlagMorePackets) {
                unsigned char *tpb = reinterpret_cast<unsigned char *>(t);
                tpb += GTransportLen;
                tpb += t->len;
                t = reinterpret_cast<GTransport *>(tpb);
            } else {
                break;
            }
        }
    }
}

void Server::event_logout(const Connection *c, LogoutReason reason) throw (Exception) {
    for (Players::iterator it = players.begin(); it != players.end(); it++) {
        Player *p = *it;
        if (p->get_connection() == c) {
            if (hold_disconnected_players) {
                if (!(p->state.server_state.flags & PlayerServerFlagSpectating)) {
                    PlayerStats *pst = new PlayerStats;
                    get_now(pst->disconnect_time);
                    pst->host = c->host;
                    pst->player_name = p->get_player_name();
                    pst->tournament_id = factory.get_tournament_id();
                    pst->server_state = p->state.server_state;
                    pst->client_server_state = p->state.client_server_state;
                    held_player_stats.push_back(pst);
                }
            }

            player_id_t id = htons(p->state.id);
            I18NText logout_id = get_logout_text_id_client(reason);

            /* inform clients */
            broadcast_data(factory.get_tournament_id(), GPCRemovePlayer, NetFlagsReliable, sizeof(id), &id);

            /* send remove to all clients if client is synced */
            if (p->client_synced) {
                /* inform tournament */
                if (tournament) {
                    tournament->player_removed(p);
                }

                /* send disappear animation */
                if (tournament) {
                    GAnimation ani;
                    memset(&ani, 0, sizeof(GAnimation));
                    strncpy(ani.animation_name, "disappear", NameLength - 1);
                    ani.id = tournament->create_animation_id();
                    ani.x = p->state.client_server_state.x + Characterset::Colbox.x;
                    ani.y = p->state.client_server_state.y - (Characterset::Height / 2.0f);
                    ani.to_net();
                    stacked_broadcast_data_synced(factory.get_tournament_id(), GPCAddAnimation, NetFlagsReliable, GAnimationLen, &ani);
                }

                /* send bye bye message */
                int msgidx = (rand() % nbr_logout_msg) + I18N_TNMT_LOGOUT_MSG01;
                Font *font = resources.get_font("normal");
                GTextAnimation tani;
                memset(&tani, 0, sizeof(GTextAnimation));
                strncpy(tani.font_name, font->get_name().c_str(), NameLength - 1);
                tani.i18n_id = msgidx;
                tani.x = p->state.client_server_state.x + (Characterset::Width / 2);
                tani.y = p->state.client_server_state.y - (Characterset::Height / 2) - 30;
                tani.max_counter = 50;
                tani.to_net();
                stacked_broadcast_data_synced(factory.get_tournament_id(), GPCAddTextAnimation, NetFlagsReliable, GTextAnimationLen, &tani);
            }

            /* flush */
            flush_stacked_broadcast_data_synced(NetFlagsReliable);

            /* send text message */
            size_t msglen;
            AutoPtr<char[]> txtptr(Tournament::create_i18n_response(logout_id, msglen, p->get_player_name().c_str()));
            GI18NText *txtmsg = reinterpret_cast<GI18NText *>(&txtptr[0]);
            broadcast_data(factory.get_tournament_id(), GPCI18NText, NetFlagsReliable, static_cast<data_len_t>(msglen), txtmsg);
            logger.log(ServerLogger::LogTypePlayerDisconnect, ClientServer::i18n(logout_id, p->get_player_name()), p);

            /* delete client pak list */
            destroy_paks(p);

            /* delete player */
            players.erase(it);
            delete p;
            break;
        }
    }

    /* if last player, close tournament */
    if (!players.size()) {
        if (tournament) {
            delete tournament;
            tournament = 0;
        }
        cleanup_held_players();
    }
}

bool Server::select_map() {
    bool switch_to_game = false;

    if (use_temporary_map_config()) {
        if (tournament) {
            delete tournament;
            tournament = 0;
        }
        current_config = &get_temporary_map_config();
        warmup = (current_config->warmup_in_seconds > 0);
        set_temporary_map_config(false);
    } else {
        if (tournament) {
            delete tournament;
            tournament = 0;
            switch_to_game = warmup;
        }

        if (switch_to_game) {
            warmup = false;
        } else {
            /* reload map rotation list */
            if (reload_map_rotation) {
                reload_map_rotation = false;
                load_map_rotation();
            }

            /* pick next map */
            const MapConfiguration& config = map_configs[rotation_current_index];
            current_config = &config;
            warmup = (config.warmup_in_seconds > 0);

            size_t sz = map_configs.size();
            rotation_current_index++;
            if (rotation_current_index >= sz) {
                rotation_current_index = 0;
            }
        }
    }

    tournament = factory.create_tournament(*current_config, true, warmup, players, &logger);
    factory.set_tournament_server_flags(*this, tournament);
    score_board_counter = 30000;

    return switch_to_game;
}

void Server::cleanup_held_players() {
    for (HeldPlayerStats::iterator it = held_player_stats.begin();
        it != held_player_stats.end(); it++)
    {
        delete *it;
    }
    held_player_stats.clear();
}

void Server::quit_client(const Connection *c, Player *p, const std::string& message) {
    send_data(c, 0, GPCServerQuit, NetFlagsReliable, message.length(), message.c_str());
    p->server_force_quit_client = true;
    p->server_force_quit_client_counter = 1000;
}

void Server::sync_client(const Connection *c, Player *p) {
    /* ----------------------------------- */
    /* INITIAL GAME STATE SETUP FOR CLIENT */
    /* ----------------------------------- */

    /* inform tournament */
    if (tournament) {
        tournament->player_added(p);
    }

    /* send current gameplay state */
    if (current_config) {
        GTournament gt;
        memset(&gt, 0, GTournamentLen);
        strncpy(gt.map_name, current_config->map_name.c_str(), NameLength - 1);
        gt.duration = static_cast<scounter_t>(current_config->duration);
        gt.warmup = static_cast<scounter_t>(current_config->warmup_in_seconds);
        gt.flags |= (warmup ? TournamentFlagWarmup : 0);
        gt.gametype = static_cast<unsigned char>(current_config->type);
        gt.tournament_id = factory.get_tournament_id();
        gt.to_net();
        stacked_send_data(c, factory.get_tournament_id(), GPCMapState, NetFlagsReliable, GTournamentLen, &gt);
    }

    /* send subclassed tournament states and flags */
    if (tournament) {
        tournament->retrieve_flags();
        tournament->retrieve_states();
        Tournament::StateResponses& responses = tournament->get_state_responses();
        size_t sz = responses.size();
        for (size_t i = 0; i < sz; i++) {
            StateResponse *resp = responses[i];
            stacked_send_data(c, factory.get_tournament_id(), resp->action, NetFlagsReliable, resp->len, resp->data);
        }
        tournament->delete_responses();
    }

    /* in game states */
    if (tournament) {
        /* send object states */
        GPlaceObject gpo;
        GSpawnObject so;
        Tournament::GameObjects& objects = tournament->get_game_objects();
        for (Tournament::GameObjects::iterator it = objects.begin();
            it != objects.end(); it++)
        {
            GameObject *obj = *it;
            if (obj->spawned_object) {
                memset(&so, 0, GSpawnObjectLen);
                strncpy(so.object_name, obj->object->get_name().c_str(), NameLength - 1);
                so.flags = 0;
                so.id = obj->state.id;
                so.x = static_cast<pos_t>(round(obj->state.x));
                so.y = static_cast<pos_t>(round(obj->state.y));
                so.to_net();
                stacked_send_data(c, factory.get_tournament_id(), GPCSpawnObject, NetFlagsReliable, GSpawnObjectLen, &so);
            } else if (!obj->picked) {
                gpo.id = obj->state.id;
                gpo.flags = 0;
                gpo.x = static_cast<pos_t>(round(obj->state.x));
                gpo.y = static_cast<pos_t>(round(obj->state.y));
                gpo.to_net();
                stacked_send_data(c, factory.get_tournament_id(), GPCPlaceObject, NetFlagsReliable, GPlaceObjectLen, &gpo);
            }
        }

        /* send spawnable npcs */
        GSpawnNPC npcspwn;
        Tournament::SpawnableNPCs& spawnable_npcs = tournament->get_spawnable_npcs();
        for (Tournament::SpawnableNPCs::iterator it = spawnable_npcs.begin();
            it != spawnable_npcs.end(); it++)
        {
            SpawnableNPC *npc = *it;

            memset(&npcspwn, 0, GSpawnNPCLen);
            strncpy(npcspwn.npc_name, npc->npc->get_name().c_str(), NameLength - 1);
            npcspwn.id = npc->state.id;
            npcspwn.owner = npc->state.owner;
            npcspwn.direction = npc->state.direction;
            npcspwn.icon = static_cast<unsigned char>(npc->icon);
            npcspwn.x = npc->state.x;
            npcspwn.y = npc->state.y;
            npcspwn.accel_x = npc->state.accel_x;
            npcspwn.accel_y = npc->state.accel_y;
            npcspwn.to_net();
            stacked_send_data(c, factory.get_tournament_id(), GPCSpawnNPC, NetFlagsReliable, GSpawnNPCLen, &npcspwn);
        }

        /* send animation states */
        GAnimation gani;
        Tournament::GameAnimations& animations = tournament->get_game_animations();
        for (Tournament::GameAnimations::iterator it = animations.begin();
            it != animations.end(); it++)
        {
            GameAnimation *ani = *it;
            if (ani->animation->get_physics()) {
                gani.id = ani->state.id;
                memset(gani.animation_name, 0, NameLength);
                strncpy(gani.animation_name, ani->animation->get_name().c_str(), NameLength - 1);
                memset(gani.sound_name, 0, NameLength);
                gani.duration = ani->state.duration;
                gani.owner = ani->state.owner;
                gani.x = ani->state.x;
                gani.y = ani->state.y;
                gani.accel_x = ani->state.accel_x;
                gani.accel_y = ani->state.accel_y;
                gani.to_net();
                stacked_send_data(c, factory.get_tournament_id(), GPCAddAnimation, NetFlagsReliable, GAnimationLen, &gani);
            }
        }

        /* flush */
        flush_stacked_send_data(c, NetFlagsReliable);
    }

    /* send generic states */
    if (tournament) {
        GenericData *gd = tournament->create_generic_data();
        GenericData *pgd = gd;
        while (pgd) {
            send_data(c, factory.get_tournament_id(), GPCGenericData, NetFlagsReliable, pgd->sz, pgd->data);
            pgd = pgd->next;
        }
        tournament->destroy_generic_data_list(gd);
    }

    /* server motd */
    const std::string& srv_msg = get_value("server_message");
    if (srv_msg.length() && srv_msg.length() <= MaxServerMsgLength) {
        send_data(c, factory.get_tournament_id(), GPCServerMessage, NetFlagsReliable, static_cast<data_len_t>(srv_msg.length()), srv_msg.c_str());
    }

    /* send ready to client */
    send_data(c, factory.get_tournament_id(), GPCReady, NetFlagsReliable, 0, 0);

    /* client is synced now */
    p->client_synced = true;
}

Server::PlayerClientPak *Server::get_player_client_pak(Player *p) {
    for (PlayerClientPaks::iterator it = player_client_paks.begin();
        it != player_client_paks.end(); it++)
    {
        PlayerClientPak *pak = *it;
        if (pak->p == p) {
            return pak;
        }
    }

    return 0;
}

void Server::process_sync_pak(const Connection *c, Player *p) throw (ServerException) {
    bool synced = false;
    PlayerClientPak *pcpak = get_player_client_pak(p);
    if (pcpak) {
        if (!pcpak->current_client_pak) {
            pcpak->current_client_pak = get_unsynced_client_pak(pcpak);
        }

        if (!pcpak->current_client_pak) {
            synced = true;
        } else {
            /* send header */
            if (!pcpak->f) {
                pcpak->f = fopen(pcpak->current_client_pak->pak->pak_name.c_str(), "rb");
                if (!pcpak->f) {
                    throw ServerException(strerror(errno));
                }
                GXferHeader header;
                memset(&header, 0, GXferHeaderLen);
                strncpy(header.filename, pcpak->current_client_pak->pak->pak_short_name.c_str(), FilenameLen - 1);
                fseek(pcpak->f, 0, SEEK_END);
                header.filesize = static_cast<datasize_t>(ftell(pcpak->f));
                pcpak->current_client_pak->remaining = header.filesize;
                fseek(pcpak->f, 0, SEEK_SET);
                header.to_net();
                send_data(c, 0, GPCXferHeader, NetFlagsReliable, GXferHeaderLen, &header);
            }

            /* send packet */
            GXferDataChunk *chunk = reinterpret_cast<GXferDataChunk *>(pak_buffer);
            datasize_t csz;
            memset(chunk, 0, sizeof(pak_buffer));
            chunk->chunksize = csz = static_cast<datasize_t>(fread(chunk->data, 1, PacketMaxSize - GXferDataChunkLen, pcpak->f));
            chunk->to_net();
            send_data(c, 0, GPCXferDataChunk, NetFlagsReliable, GXferDataChunkLen + csz, chunk);

            /* last packet? */
            pcpak->current_client_pak->remaining -= csz;
            if (!pcpak->current_client_pak->remaining) {
                fclose(pcpak->f);
                pcpak->f = 0;
                pcpak->current_client_pak->synced = true;
                pcpak->current_client_pak = 0;
            }
        }
    } else {
        synced = true;
    }

    /* all files are synced, now sync player game state */
    if (synced) {
        sync_client(c, p);
    }
}

Server::ClientPak *Server::get_unsynced_client_pak(PlayerClientPak *pcpak) {
    for (ClientPaks::iterator it = pcpak->client_paks.begin();
        it != pcpak->client_paks.end(); it++)
    {
        ClientPak& pak = *it;
        if (!pak.synced) {
            return &pak;
        }
    }

    return 0;
}

void Server::destroy_paks(Player *p) {
    /* delete complete client pak list */
    for (PlayerClientPaks::iterator pit = player_client_paks.begin();
        pit != player_client_paks.end(); pit++)
    {
        PlayerClientPak *pak = *pit;
        if (!p || pak->p == p) {
            if (pak->f) {
                fclose(pak->f);
            }
            delete pak;
            if (p) {
                player_client_paks.erase(pit);
                break;
            }
        }
    }
}

void Server::load_map_rotation() {
    map_configs.clear();
    char kvb[128];
    int map_count = atoi(get_value("map_count").c_str());
    if (map_count < 1) {
        throw ServerException(ClientServer::i18n(I18N_NO_MAPS_DEFINED));
    }

    for (int i = 0; i < map_count; i++) {
        sprintf(kvb, "name%d", i);
        const std::string& map_name = get_value(kvb);

        sprintf(kvb, "type%d", i);
        const std::string map_type_str = get_value(kvb);
        GamePlayType type = GamePlayTypeDM;
        if (map_type_str.length()) {
            type = static_cast<GamePlayType>(atoi(get_value(kvb).c_str()));
        } else {
            type = resources.get_map(map_name)->get_game_play_type();
        }

        sprintf(kvb, "duration%d", i);
        int duration = atoi(get_value(kvb).c_str());
        sprintf(kvb, "warmup%d", i);
        int warmup = atoi(get_value(kvb).c_str());
        map_configs.push_back(MapConfiguration(type, map_name, duration, warmup));
    }
    if (atoi(get_value("shuffle_maps").c_str())) {
        std::random_shuffle(map_configs.begin(), map_configs.end());
    }
}

std::ostream& Server::create_log_stream() {
    const std::string& log_filename = get_value("logfile");
    if (log_filename.length()) {
        log_file = new std::ofstream(log_filename.c_str(), std::ofstream::out | std::ofstream::app);
        if (log_file->is_open()) {
            return *log_file;
        }
        subsystem << "can't open logfile " << log_filename << " (" << strerror(errno) << ")" << std::endl;
        delete log_file;
        log_file = 0;
    }
    return subsystem.get_stream();
}

void Server::parse_command(const Connection *c, Player *p, data_len_t len, void *data) throw (ServerAdminException) {
    char *pcmd = static_cast<char *>(data);
    std::string command(&pcmd[1], len - 1);
    std::string param;
    size_t pos = command.find(' ');
    if (pos != std::string::npos) {
        param = command.substr(pos + 1);
        command = command.substr(0, pos);
    }

    execute(c, p, command, param);
}

void Server::setup_loaded_paks() {
    const Resources::LoadedPaks& loaded_paks = resources.get_loaded_paks();
    for (Resources::LoadedPaks::const_iterator it = loaded_paks.begin(); it != loaded_paks.end(); it++) {
        const Resources::LoadedPak& loaded_pak = *it;
        const Resources::NonDownloadableMainPak *ptr = Resources::NonDownloadableMainPaks;
        bool check = true;
        while (ptr->name) {
            if (!strcmp(ptr->name, loaded_pak.pak_short_name.c_str())) {
                check = ptr->check;
                break;
            }
            ptr++;
        }
        if (check) {
            my_paks.push_back(loaded_pak);
        }
    }
}

ScopeServer::ScopeServer(Server& server) : server(server) {
    server.start();
}

ScopeServer::~ScopeServer() {
    server.stop();
}
