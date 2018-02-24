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

#include "Client.hpp"
#include "Scope.hpp"
#include "ScopeAllocator.hpp"

#include <cerrno>

void Client::event_status(hostaddr_t host, hostport_t port, const std::string& name,
    int max_clients, int cur_clients, ms_t ping_time, bool secured,
    int protocol_version)
{
    if (protocol_version != ProtocolVersion) {
        size_t sz;
        char *p = create_text(ClientServer::i18n(I18N_VERSION_MISMATCH_MESSAGE), sz);
        server_events.push(ServerEvent(EventTypeAccessDenied, 0, p, sz));
    } else {
        std::string msg(ClientServer::i18n(I18N_CLIENT_CONNECTING_TO1, name, ping_time));
        if (secured) {
            msg += ClientServer::i18n(I18N_CLIENT_CONNECTING_TO2);
        }
        msg += " " + ClientServer::i18n(I18N_CLIENT_CONNECTING_TO3, cur_clients, max_clients);;
        size_t sz;
        char *p = create_text(msg, sz);
        server_events.push(ServerEvent(EventTypeStatus, 0, p, sz));
        factory.set_server_name(tournament, name);
    }
}

void Client::event_access_denied(MessageSequencer::RefusalReason reason) {
    const char *msg = 0;
    std::string str;
    switch (reason) {
        case MessageSequencer::RefusalReasonServerFull:
            str = ClientServer::i18n(I18N_SERVER_FULL);
            msg = str.c_str();
            break;

        case MessageSequencer::RefusalReasonWrongPassword:
            str = ClientServer::i18n(I18N_WRONG_PASSWORD);
            msg = str.c_str();
            break;

        case MessageSequencer::RefusalReasonWrongProtocol:
            str = ClientServer::i18n(I18N_VERSION_MISMATCH_MESSAGE);
            msg = str.c_str();
            break;
    }
    if (msg) {
        size_t sz;
        char *p = create_text(msg, sz);
        server_events.push(ServerEvent(EventTypeAccessDenied, 0, p, sz));
    }
}

void Client::event_login(const Connection *c, data_len_t len, void *data) {
    logged_in = true;
    conn = c;
    size_t sz;
    char *p = create_text(ClientServer::i18n(I18N_CLIENT_LOGGED_IN), sz);
    server_events.push(ServerEvent(EventTypeLogin, c, p, sz));
}

void Client::event_data(const Connection *c, data_len_t len, void *data) {
    if (len) {
        char *p = new char[len];
        memcpy(p, data, len);
        server_events.push(ServerEvent(EventTypeData, c, p, len));
    }
}

void Client::event_logout(const Connection *c, LogoutReason reason) {
    size_t sz;
    char *p = create_text(ClientServer::i18n(get_logout_text_id(reason)), sz);
    server_events.push(ServerEvent(EventTypeLogout, c, p, sz));
}

void Client::sevt_login(ServerEvent& evt) {
    Scope<Mutex> lock(mtx);

    const Resources::LoadedPaks& paks = resources.get_loaded_paks();
    GPakHash gph;
    for (Resources::LoadedPaks::const_iterator it = paks.begin(); it != paks.end(); it++) {
        const Resources::LoadedPak& pak = *it;
        if (pak.pak_short_name.length() > 31) {
            throw ClientException(ClientServer::i18n(I18N_PAK_NAME_TOO_LONG, pak.pak_short_name));
        }
        memset(&gph, 0, GPakHashLen);
        strncpy(gph.pak_name, pak.pak_short_name.c_str(), NameLength - 1);
        strncpy(gph.pak_hash, pak.pak_hash.c_str(), GPakHash::HashLength);
        gph.to_net();
        stacked_send_data(evt.c, 0, GPSPakSyncHash, NetFlagsReliable, GPakHashLen, &gph);
    }
    stacked_send_data(evt.c, 0, GPSPakSyncHashFinished, NetFlagsReliable, 0, 0);
    flush_stacked_send_data(evt.c, NetFlagsReliable);
}

void Client::sevt_data(ServerEvent& evt) {
    GTransport *t = reinterpret_cast<GTransport *>(evt.data);

    while (true) {
        t->from_net();

#ifdef SAFE_ALIGNMENT
        data_t *data_ptr = aligned_buffer;
        memcpy(data_ptr, t->data, t->len);
#else
        data_t *data_ptr = t->data;
#endif
        switch (t->cmd) {
            case GPCServerMessage:
            {
                size_t sz = 0;
                std::string msg(reinterpret_cast<char *>(data_ptr), t->len);
                std::vector<std::string> lines;
                while (true) {
                    sz++;
                    size_t pos = msg.find("|");
                    if (pos == std::string::npos) {
                        lines.push_back(msg);
                        break;
                    }
                    lines.push_back(msg.substr(0, pos));
                    msg = msg.substr(pos + 1);
                }
                int vw = subsystem.get_view_width();
                int vh = subsystem.get_view_height();
                int ww = 200;
                int wh = 130;
                int x = 10;
                int y = 10;
                GuiWindow *window = push_window(x, y, ww, wh, ClientServer::i18n(I18N_CLIENT_SERVER_MESSAGE));

                Font *f = get_font();
                int text_height = 0;
                int ly = Spc;
                int maxw = 0;
                for (size_t i = 0; i < sz; i++) {
                    create_label(window, Spc, ly, lines[i]);
                    ly += f->get_font_height();
                    text_height += f->get_font_height();
                    int tw = f->get_text_width(lines[i]) + 2 * Spc;
                    maxw = (tw > maxw ? tw : maxw);
                }
                int maxh = text_height + 2 * Spc;
                int bh = 18;
                std::string btn_ok(ClientServer::i18n(I18N_BUTTON_OK));
                int bw_ok = get_font()->get_text_width(btn_ok) + 28;
                create_button(window, maxw / 2 - bw_ok / 2, maxh, bw_ok, bh, btn_ok, static_window_close_click, this);
                maxh += bh + Spc;
                window->set_width(window->get_width() - window->get_client_width() + maxw);
                window->set_height(window->get_height() - window->get_client_height() + maxh);
                window->set_x(vw / 2 - window->get_width() / 2);
                window->set_y(vh / 2 - window->get_height() / 2);
                break;
            }

            case GPCMapState:
            {
                GTournament *tour = reinterpret_cast<GTournament *>(data_ptr);
                tour->from_net();
                if (tournament) {
                    delete tournament;
                }

                /* reload resources? */
                if (reload_resources) {
                    resources.reload_resources();
                    load_resources();
                    reload_resources = false;
                }

                /* setup tournament */
                bool warmup = ((tour->flags & TournamentFlagWarmup) != 0);
                bool leave_lobby = ((tour->flags & TournamentFlagNotInLobby) != 0);
                GamePlayType type = static_cast<GamePlayType>(tour->gametype);
                MapConfiguration config(type, tour->map_name, tour->duration, tour->warmup);
                tournament = factory.create_tournament(config, false, warmup, players, 0);
                factory.set_tournament_id(tour->tournament_id);
                tournament->set_following_id(my_id);
                tournament->set_player_configuration(&player_config);
                tournament->set_lagometer(show_lagometer ? &lagometer : 0);
                if (leave_lobby) {
                    tournament->leave_lobby();
                } else {
                    subsystem.play_sound(resources.get_sound("lobby"), 0);
                }
                add_text_msg(ClientServer::i18n(I18N_CLIENT_MAP_INFO, tournament->get_map().get_description()));

                /* reopen, if join request window is already open */
                if (me && me->joining) {
                    tournament->reopen_join_window(me);
                }
                break;
            }

            case GPCIdentifyPlayer:
            {
                player_id_t *nid = reinterpret_cast<player_id_t *>(data_ptr);
                my_id = ntohs(*nid);
                if (tournament) {
                    tournament->set_following_id(my_id);
                }
                break;
            }

            case GPCReady:
            {
                if (tournament) {
                    tournament->set_ready();
                }
                if (me) {
                    me->state.client_server_state.key_states = 0;
                }
                break;
            }

            case GPCAddPlayer:
            {
                GPlayerInfo *info = reinterpret_cast<GPlayerInfo *>(data_ptr);
                info->from_net();

                Player *p = new Player(resources, 0, info->id,
                    info->desc.player_name, info->desc.characterset_name);
                players.push_back(p);

                p->state.server_state = info->server_state;
                p->state.client_server_state = info->client_server_state;
                p->state.client_state = info->client_state;

                if (p->state.id == my_id) {
                    me = p;
                    p->mark_as_me();
                }

                if (tournament) {
                    tournament->player_added(p);
                }

                break;
            }

            case GPCResetPlayer:
            {
                GPlayerInfo *info = reinterpret_cast<GPlayerInfo *>(data_ptr);
                info->from_net();

                for (Players::iterator it = players.begin();
                    it != players.end(); it++)
                {
                    Player *p = *it;
                    if (p->state.id == info->id) {
                        p->state.server_state = info->server_state;
                        p->state.client_server_state = info->client_server_state;
                        p->state.client_state = info->client_state;
                        if (tournament) {
                            tournament->reset_player(p);
                        }
                        break;
                    }
                }
                break;
            }

            case GPCRemovePlayer:
            {
                player_id_t *nid = reinterpret_cast<player_id_t *>(data_ptr);
                player_id_t id = ntohs(*nid);
                for (Players::iterator it = players.begin();
                    it != players.end(); it++)
                {
                    Player *p = *it;
                    if (p->state.id == id) {
                        if (id == my_id) {
                            me = 0;
                        }

                        if (tournament) {
                            tournament->player_removed(p);
                        }

                        players.erase(it);
                        delete p;
                        break;
                    }
                }
                break;
            }

            case GPCSpawnPlayer:
            {
                GPlayerState *state = reinterpret_cast<GPlayerState *>(data_ptr);
                state->from_net();
                for (Players::iterator it = players.begin();
                    it != players.end(); it++)
                {
                    Player *p = *it;
                    if (p->state.id == state->id) {
                        if (p == me) {
                            p->state.server_state = state->server_state;
                        } else {
                            p->state = *state;
                        }
                        p->respawning = false;
                        if (tournament) {
                            tournament->add_player_spawn_animation(p);
                        }
                        break;
                    }
                }
                break;
            }

            case GPCUpdateGameState:
            {
                if (t->tournament_id == factory.get_tournament_id()) {
                    GGameState *state = reinterpret_cast<GGameState *>(data_ptr);
                    state->from_net();
                    if (tournament) {
                        tournament->get_game_state() = *state;
                    }
                }
                break;
            }

            case GPCUpdatePlayerState:
            {
                if (t->tournament_id == factory.get_tournament_id()) {
                    GPTAllStates *state = reinterpret_cast<GPTAllStates *>(data_ptr);
                    state->from_net();

                    for (Players::iterator it = players.begin();
                        it != players.end(); it++)
                    {
                        Player *p = *it;
                        if (p->state.id == state->id) {
                            p->state.server_state = state->server_state;
                            if (p != me) {
                                p->state.client_server_state = state->client_server_state;
                            } else {
                                if (tournament) {
                                    if (show_lagometer) {
                                        lagometer.update(p->state.server_state.ping_time, p->state.server_state.outq_sz, get_outq_sz(conn));
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
                break;
            }

            case GPCUpdateObject:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        GObjectState *state = reinterpret_cast<GObjectState *>(data_ptr);
                        state->from_net();
                        Tournament::GameObjects& objects = tournament->get_game_objects();
                        for (Tournament::GameObjects::iterator it = objects.begin();
                            it != objects.end(); it++)
                        {
                            GameObject *obj = *it;
                            if (obj->state.id == state->id) {
                                obj->state = *state;
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case GPCUpdateAnimation:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        GAnimationState *state = reinterpret_cast<GAnimationState *>(data_ptr);
                        state->from_net();
                        Tournament::GameAnimations& animations = tournament->get_game_animations();
                        for (Tournament::GameAnimations::iterator it = animations.begin();
                            it != animations.end(); it++)
                        {
                            GameAnimation *ani = *it;
                            if (ani->state.id == state->id) {
                                ani->state = *state;
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case GPCUpdateSpawnableNPC:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        GNPCState *state = reinterpret_cast<GNPCState *>(data_ptr);
                        state->from_net();
                        Tournament::SpawnableNPCs& spawnable_npcs = tournament->get_spawnable_npcs();
                        for (Tournament::SpawnableNPCs::iterator it = spawnable_npcs.begin();
                            it != spawnable_npcs.end(); it++)
                        {
                            SpawnableNPC *npc = *it;
                            if (npc->state.id == state->id) {
                                npc->state = *state;
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case GPCSpawnNPC:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        GSpawnNPC *state = reinterpret_cast<GSpawnNPC *>(data_ptr);
                        state->from_net();
                        tournament->add_spawnable_npc(state);
                    }
                }
                break;
            }

            case GPCRemoveNPC:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        GRemoveNPC *rnpc = reinterpret_cast<GRemoveNPC *>(data_ptr);
                        rnpc->from_net();
                        tournament->remove_spawnable_npc(rnpc);
                    }
                }
                break;
            }

            case GPCTextMessage:
            {
                std::string msg(reinterpret_cast<char *>(data_ptr), t->len);
                add_text_msg(msg);
                break;
            }

            case GPCPlaySound:
            {
                GGenericName *name = reinterpret_cast<GGenericName *>(data_ptr);
                try {
                    subsystem.play_sound(resources.get_sound(name->name), 0);
                } catch (...) {
                    /* chomp */
                }
                break;
            }

            case GPCChatMessage:
            {
                GChatMessage *chat_msg = reinterpret_cast<GChatMessage *>(data_ptr);
                chat_msg->from_net();
                for (Players::iterator it = players.begin(); it != players.end(); it++) {
                    Player *p = *it;
                    if (p->state.id == chat_msg->id) {
                        std::string msg(reinterpret_cast<char *>(chat_msg->data), chat_msg->len);
                        add_text_msg(chat_icon, p->get_player_name(), msg);
                        subsystem.play_sound(resources.get_sound("chat"), 0);
                        break;
                    }
                }
                break;
            }

            case GPCAddAnimation:
            {
                GAnimation *ani = reinterpret_cast<GAnimation *>(data_ptr);
                ani->from_net();
                if (tournament) {
                    tournament->add_animation(ani);
                }
                break;
            }

            case GPCAddTextAnimation:
            {
                GTextAnimation *ani = reinterpret_cast<GTextAnimation *>(data_ptr);
                ani->from_net();
                if (tournament) {
                    tournament->add_text_animation(ani);
                }
                break;
            }

            case GPCPlayerRecoil:
            {
                GPlayerRecoil *prec = reinterpret_cast<GPlayerRecoil *>(data_ptr);
                prec->from_net();
                for (Players::iterator it = players.begin(); it != players.end(); it++) {
                    Player *p = *it;
                    if (p->state.id == prec->id) {
                        p->state.client_server_state.accel_x += prec->x_recoil;
                        break;
                    }
                }
                break;
            }

            case GPCPlayerHurt:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        subsystem.play_sound(resources.get_sound("hurt"), 0);
                    }
                }
                break;
            }

            case GPCPickObject:
            {
                GPickObject *po = reinterpret_cast<GPickObject *>(data_ptr);
                po->from_net();
                if (tournament) {
                    tournament->add_pick_object(po);
                }
                break;
            }

            case GPCPlaceObject:
            {
                GPlaceObject *po = reinterpret_cast<GPlaceObject *>(data_ptr);
                po->from_net();
                if (tournament) {
                    tournament->add_place_object(po);
                }
                break;
            }

            case GPCSpawnObject:
            {
                GSpawnObject *so = reinterpret_cast<GSpawnObject *>(data_ptr);
                so->from_net();
                if (tournament) {
                    tournament->spawn_object(so);
                }
                break;
            }

            case GPCJoinAccepted:
            {
                if (tournament) {
                    tournament->join_accepted();
                }
                break;
            }

            case GPCJoinRefused:
            {
                if (tournament) {
                    tournament->join_refused();
                    add_text_msg(ClientServer::i18n(I18N_CLIENT_JOIN_REFUSED));
                    subsystem.play_sound(resources.get_sound("error"), 0);
                }
                break;
            }

            case GPCTeamScore:
            {
                GTeamScore *ts = reinterpret_cast<GTeamScore *>(data_ptr);
                ts->from_net();
                if (tournament) {
                    tournament->add_team_score(ts);
                }
                break;
            }

            case GPCTimeRemaining:
            {
                GTimeRemaining *remain = reinterpret_cast<GTimeRemaining *>(data_ptr);
                remain->from_net();
                if (tournament) {
                    tournament->update_wearable_remaining(remain);
                }
                break;
            }

            case GPCLeaveLobby:
            {
                if (tournament) {
                    tournament->leave_lobby();
                }
                break;
            }

            case GPCFriendlyFire:
            {
                GFriendyFireAlarm *alarm = reinterpret_cast<GFriendyFireAlarm *>(data_ptr);
                alarm->from_net();
                if (tournament) {
                    if (tournament->friendly_fire_alarm(alarm)) {
                        subsystem.play_controlled_sound(resources.get_sound("friendly_fire"), 0);
                        add_text_msg(ClientServer::i18n(I18N_CLIENT_FRIENDLY_FIRE));
                    }
                }
                break;
            }

            case GPCGamePlayUnbalanced:
            {
                if (tournament) {
                    subsystem.play_controlled_sound(resources.get_sound("unbalanced"), 0);
                    add_text_msg(ClientServer::i18n(I18N_CLIENT_UNBALANCED));
                }
                break;
            }

            case GPCWarmUp:
            {
                if (tournament) {
                    subsystem.play_system_sound(resources.get_sound("warm_up"));
                    add_text_msg(ClientServer::i18n(I18N_CLIENT_WARMUP));
                }
                break;
            }

            case GPCGameBegins:
            {
                if (tournament) {
                    subsystem.play_system_sound(resources.get_sound("ready"));
                    add_text_msg(ClientServer::i18n(I18N_CLIENT_GAME_BEGINS));
                }
                break;
            }

            case GPCGameOver:
            {
                if (tournament) {
                    subsystem.play_system_sound(resources.get_sound("game_over"));
                    add_text_msg(ClientServer::i18n(I18N_CLIENT_GAME_OVER));
                }
                update_options_window();
                break;
            }

            case GPCPlayerChanged:
            {
                GPlayerDescription *pdesc = reinterpret_cast<GPlayerDescription *>(data_ptr);
                pdesc->from_net();
                for (Players::iterator it = players.begin(); it != players.end(); it++) {
                    Player *p = *it;
                    if (p->state.id == pdesc->id) {
                        std::string old_name = p->get_player_name();
                        std::string new_name(pdesc->player_name);
                        std::string old_skin(p->get_characterset_name());
                        std::string new_skin(pdesc->characterset_name);

                        /* change player name */
                        if (old_name != new_name) {
                            p->set_player_name(new_name);
                            p->font = 0;
                            add_text_msg(ClientServer::i18n(I18N_CLIENT_RENAME, old_name, new_name));
                        }

                        /* change skin */
                        if (old_skin != new_skin) {
                            p->set_characterset(new_skin);
                            add_text_msg(ClientServer::i18n(I18N_CLIENT_SKIN_CHANGE, p->get_player_name(), p->get_characterset_name()));
                        }
                        break;
                    }
                }
                break;
            }

            case GPCScoreTransportRaw:
            {
                if (tournament) {
                    tournament->score_transport_raw(data_ptr);
                }
                break;
            }

            case GPCRemoveAnimation:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        identifier_t *id = reinterpret_cast<identifier_t *>(data_ptr);
                        tournament->remove_animation(ntohs(*id));
                    }
                }
                break;
            }

            case GPCXferHeader:
            {
                reload_resources = true;
                GXferHeader *header = reinterpret_cast<GXferHeader *>(data_ptr);
                header->from_net();
                if (!fhnd) {
                    xfer_filename = header->filename;
                    current_download_filename = std::string(resources.get_path_manager().get_data_home()) + dir_separator + header->filename;
                    fhnd = fopen(current_download_filename.c_str(), "wb");
                    if (fhnd) {
                        total_xfer_sz = remaining_xfer_sz = header->filesize;
                    } else {
                        subsystem << ClientServer::i18n(I18N_XFER_OPEN_FAILED, strerror(errno)) << std::endl;
                    }
                } else {
                    subsystem << ClientServer::i18n(I18N_XFER_ALREADY_OPEN, xfer_filename) << std::endl;
                }
                break;
            }

            case GPCXferDataChunk:
            {
                GXferDataChunk *chunk = reinterpret_cast<GXferDataChunk *>(data_ptr);
                chunk->from_net();
                if (fhnd) {
                    fwrite(chunk->data, chunk->chunksize, 1, fhnd);
                    remaining_xfer_sz -= chunk->chunksize;
                    if (remaining_xfer_sz < 1) {
                        fclose(fhnd);
                        fhnd = 0;
                        current_download_filename = "";
                    }
                }
                {
                    Scope<Mutex> lock(mtx);
                    send_data(evt.c, 0, GPSPakSyncAck, NetFlagsReliable, 0, 0);
                }
                break;
            }

            case GPCGenericData:
            {
                if (tournament) {
                    tournament->generic_data_delivery(data_ptr);
                }
                break;
            }

            case GPCServerQuit:
            {
                exception_msg.assign(reinterpret_cast<char *>(data_ptr), t->len);
                throw_exception = true;
                return;
                /* not necessary */
                break;
            }

            case GPCSpectate:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        player_id_t *nid = reinterpret_cast<player_id_t *>(data_ptr);
                        player_id_t id = ntohs(*nid);

                        for (Players::iterator it = players.begin();
                            it != players.end(); it++)
                        {
                            Player *p = *it;
                            if (p->state.id == id) {
                                tournament->player_removed(p);
                                tournament->player_added(p);
                                if (id == my_id) {
                                    tournament->spectate_accepted();
                                    update_options_window();
                                }
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case GPCI18NText:
            {
                GI18NText *i18ntext = reinterpret_cast<GI18NText *>(data_ptr);
                i18ntext->from_net();
                std::string txt(ClientServer::i18n(static_cast<I18NText>(i18ntext->id)));
                if (i18ntext->len) {
                    std::string parms(reinterpret_cast<char *>(i18ntext->data), i18ntext->len);
                    std::string::size_type start_pos = 0;
                    bool again = true;
                    int idx = 1;
                    do {
                        std::string::size_type find_pos = parms.find('\t', start_pos);
                        if (find_pos == std::string::npos) {
                            find_pos = parms.length();
                            again = false;
                        }
                        ClientServer::i18n.replace(txt, idx++, parms.substr(start_pos, find_pos - start_pos));
                        start_pos += find_pos + 1;
                    } while (again);
                }
                add_text_msg(txt);
                break;
            }

            case GPCTournamentSetting:
            {
                if (tournament) {
                    if (t->tournament_id == factory.get_tournament_id()) {
                        GTournamentSetting *ts = reinterpret_cast<GTournamentSetting *>(data_ptr);
                        tournament->set_flag(static_cast<Tournament::Setting>(ts->setting_id), (ts->flag ? true : false));
                    }
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

void Client::update_options_window() {
    if (are_options_visible()) {
        int x;
        int y;
        get_options_window_position(x, y);
        refresh_options(true, x, y);
    }
}
