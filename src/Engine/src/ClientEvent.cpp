#include "Client.hpp"

#include <cerrno>

void Client::event_status(hostaddr_t host, hostport_t port, const std::string& name,
    int max_clients, int cur_clients, ms_t ping_time, bool secured,
    int protocol_version) throw (Exception)
{
    sprintf(buffer, "%ld", ping_time);
    std::string msg = "connecting to " + name + ", ping = ";
    msg += buffer;
    if (secured) {
        msg += ", needs password";
    }
    sprintf(buffer, " (clients: %d/%d)", cur_clients, max_clients);
    msg += buffer;
    size_t sz = msg.length();
    char *p = new char[sz];
    memcpy(p, msg.c_str(), sz);
    server_events.push(ServerEvent(EventTypeStatus, 0, p, sz));
}

void Client::event_access_denied(MessageSequencer::RefusalReason reason) throw (Exception) {
    const char *msg = 0;
    switch (reason) {
        case MessageSequencer::RefusalReasonServerFull:
            msg = "Login failed: server is full.";
            break;

        case MessageSequencer::RefusalReasonWrongPassword:
            msg = "Login failed: wrong password.";
            break;

    }
    if (msg) {
        size_t sz = strlen(msg);
        char *p = new char[sz];
        memcpy(p, msg, sz);
        server_events.push(ServerEvent(EventTypeAccessDenied, 0, p, sz));
    }

    throw_exception = true;
}

void Client::event_login(const Connection *c, data_len_t len, void *data) throw (Exception) {
    logged_in = true;
    conn = c;
    const char *msg = "you logged in";
    size_t sz = strlen(msg);
    char *p = new char[sz];
    memcpy(p, msg, sz);
    server_events.push(ServerEvent(EventTypeLogin, c, p, sz));
}

void Client::event_data(const Connection *c, data_len_t len, void *data) throw (Exception) {
    if (len) {
        char *p = new char[len];
        memcpy(p, data, len);
        server_events.push(ServerEvent(EventTypeData, c, p, len));
    }
}

void Client::event_logout(const Connection *c, LogoutReason reason) throw (Exception) {
    std::string msg("You logged out.");
    switch (reason) {
        case LogoutReasonRegular:
            break;

        case LogoutReasonPingTimeout:
            msg += " (ping timeout)";
            break;

        case LogoutReasonTooManyResends:
            msg += " (too many resends)";
            break;

        case LogoutApplicationQuit:
            msg += " (application layer quit)";
            break;
    }

    size_t sz = msg.length();
    char *p = new char[sz];
    memcpy(p, msg.c_str(), sz);
    server_events.push(ServerEvent(EventTypeLogout, c, p, sz));
}

void Client::sevt_login(ServerEvent& evt) {
    ScopeMutex lock(mtx);

    const Resources::LoadedPaks& paks = resources.get_loaded_paks();
    GPakHash gph;
    for (Resources::LoadedPaks::const_iterator it = paks.begin(); it != paks.end(); it++) {
        const Resources::LoadedPak& pak = *it;
        memset(&gph, 0, GPakHashLen);
        strncpy(gph.pak_name, pak.pak_short_name.c_str(), NameLength - 1);
        strncpy(gph.pak_hash, pak.pak_hash.c_str(), 64);
        gph.to_net();
        stacked_send_data(evt.c, 0, GPSPakSyncHash, NetFlagsReliable, GPakHashLen, &gph);
    }
    stacked_send_data(evt.c, 0, GPSPakSyncHashFinished, NetFlagsReliable, 0, 0);
    flush_stacked_send_data(evt.c, 0);
}

void Client::sevt_data(ServerEvent& evt) {
    GTransport *t = reinterpret_cast<GTransport *>(evt.data);

    while (true) {
        t->from_net();
        switch (t->cmd) {
            case GPCServerMessage:
            {
                size_t sz = 0;
                std::string msg(reinterpret_cast<char *>(t->data), t->len);
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
                GuiWindow *window = push_window(x, y, ww, wh, "Server Message");

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
                int bw = 46;
                int bh = 18;
                create_button(window, maxw / 2 - bw / 2, maxh, bw, bh, "Okay", static_window_close_click, this);
                maxh += bh + Spc;
                window->set_width(window->get_width() - window->get_client_width() + maxw);
                window->set_height(window->get_height() - window->get_client_height() + maxh);
                window->set_x(vw / 2 - window->get_width() / 2);
                window->set_y(vh / 2 - window->get_height() / 2);
                break;
            }

            case GPCMapState:
            {
                GTournament *tour = reinterpret_cast<GTournament *>(t->data);
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
                GamePlayType type = static_cast<GamePlayType>(tour->gametype);
                MapConfiguration config(type, tour->map_name, tour->duration, tour->warmup);
                tournament = factory.create_tournament(config, false, warmup, players, 0);
                factory.set_tournament_id(tour->tournament_id);
                tournament->set_following_id(my_id);
                tournament->set_player_configuration(&player_config);
                tournament->set_team_names(team_red_name, team_blue_name);

                /* reopen, if join request window is already open */
                if (me && me->joining) {
                    tournament->reopen_join_window(me);
                }

                break;
            }

            case GPCIdentifyPlayer:
            {
                player_id_t *nid = reinterpret_cast<player_id_t *>(t->data);
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
                GPlayerInfo *info = reinterpret_cast<GPlayerInfo *>(t->data);
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
                GPlayerInfo *info = reinterpret_cast<GPlayerInfo *>(t->data);
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
                player_id_t *nid = reinterpret_cast<player_id_t *>(t->data);
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
                GPlayerState *state = reinterpret_cast<GPlayerState *>(t->data);
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
                    GGameState *state = reinterpret_cast<GGameState *>(t->data);
                    state->from_net();
                    if (tournament) {
                        tournament->get_game_state() = *state;
                    }
                }
                break;
            }

            case GPCUpdatePlayerState:
            {
                if (t->tournament_id== factory.get_tournament_id()) {
                    GPTAllStates *state = reinterpret_cast<GPTAllStates *>(t->data);
                    state->from_net();

                    for (Players::iterator it = players.begin();
                        it != players.end(); it++)
                    {
                        Player *p = *it;
                        if (p->state.id == state->id) {
                            p->state.server_state = state->server_state;
                            if (p != me) {
                                p->state.client_server_state = state->client_server_state;
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
                        GObjectState *state = reinterpret_cast<GObjectState *>(t->data);
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
                        GAnimationState *state = reinterpret_cast<GAnimationState *>(t->data);
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
                        GNPCState *state = reinterpret_cast<GNPCState *>(t->data);
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
                        GSpawnNPC *state = reinterpret_cast<GSpawnNPC *>(t->data);
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
                        GRemoveNPC *rnpc = reinterpret_cast<GRemoveNPC *>(t->data);
                        rnpc->from_net();
                        tournament->remove_spawnable_npc(rnpc);
                    }
                }
                break;
            }

            case GPCTextMessage:
            {
                std::string msg(reinterpret_cast<char *>(t->data), t->len);
                ClientTextMessage *cmsg = new ClientTextMessage;
                cmsg->text = msg;
                client_text_messages.push_back(cmsg);
                break;
            }

            case GPCPlaySound:
            {
                GGenericName *name = reinterpret_cast<GGenericName *>(t->data);
                try {
                    subsystem.play_sound(resources.get_sound(name->name), 0);
                } catch (...) {
                    /* chomp */
                }
                break;
            }

            case GPCChatMessage:
            {
                std::string msg(reinterpret_cast<char *>(t->data), t->len);
                ClientTextMessage *cmsg = new ClientTextMessage;
                cmsg->text = msg;
                client_text_messages.push_back(cmsg);
                subsystem.play_sound(resources.get_sound("chat"), 0);
                break;
            }

            case GPCAddAnimation:
            {
                GAnimation *ani = reinterpret_cast<GAnimation *>(t->data);
                ani->from_net();
                if (tournament) {
                    tournament->add_animation(ani);
                }
                break;
            }

            case GPCAddTextAnimation:
            {
                GTextAnimation *ani = reinterpret_cast<GTextAnimation *>(t->data);
                ani->from_net();
                if (tournament) {
                    tournament->add_text_animation(ani);
                }
                break;
            }

            case GPCPlayerRecoil:
            {
                GPlayerRecoil *prec = reinterpret_cast<GPlayerRecoil *>(t->data);
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
                GPickObject *po = reinterpret_cast<GPickObject *>(t->data);
                po->from_net();
                if (tournament) {
                    tournament->add_pick_object(po);
                }
                break;
            }

            case GPCPlaceObject:
            {
                GPlaceObject *po = reinterpret_cast<GPlaceObject *>(t->data);
                po->from_net();
                if (tournament) {
                    tournament->add_place_object(po);
                }
                break;
            }

            case GPCSpawnObject:
            {
                GSpawnObject *so = reinterpret_cast<GSpawnObject *>(t->data);
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
                    ClientTextMessage *cmsg = new ClientTextMessage;
                    cmsg->text = "YOUR JOIN REQUEST WAS REFUSED";
                    client_text_messages.push_back(cmsg);
                    subsystem.play_sound(resources.get_sound("error"), 0);
                }
                break;
            }

            case GPCTeamScore:
            {
                GTeamScore *ts = reinterpret_cast<GTeamScore *>(t->data);
                ts->from_net();
                if (tournament) {
                    tournament->add_team_score(ts);
                }
                break;
            }

            case GPCTimeRemaining:
            {
                GTimeRemaining *remain = reinterpret_cast<GTimeRemaining *>(t->data);
                remain->from_net();
                if (tournament) {
                    tournament->update_wearable_remaining(remain);
                }
                break;
            }

            case GPCFriendlyFire:
            {
                GFriendyFireAlarm *alarm = reinterpret_cast<GFriendyFireAlarm *>(t->data);
                alarm->from_net();
                if (tournament) {
                    if (tournament->friendly_fire_alarm(alarm)) {
                        subsystem.play_controlled_sound(resources.get_sound("friendly_fire"), 0);
                        ClientTextMessage *cmsg = new ClientTextMessage;
                        cmsg->text = "FRIENDLY FIRE: WATCH OUT!!!";
                        client_text_messages.push_back(cmsg);
                    }
                }
                break;
            }

            case GPCGamePlayUnbalanced:
            {
                if (tournament) {
                    subsystem.play_controlled_sound(resources.get_sound("unbalanced"), 0);
                    ClientTextMessage *cmsg = new ClientTextMessage;
                    cmsg->text = "GAMEPLAY IS UNBALANCED";
                    client_text_messages.push_back(cmsg);
                }
                break;
            }

            case GPCWarmUp:
            {
                if (tournament) {
                    subsystem.play_system_sound(resources.get_sound("warm_up"));
                    ClientTextMessage *cmsg = new ClientTextMessage;
                    cmsg->text = "please warm up";
                    client_text_messages.push_back(cmsg);
                }
                break;
            }

            case GPCGameBegins:
            {
                if (tournament) {
                    subsystem.play_system_sound(resources.get_sound("ready"));
                    ClientTextMessage *cmsg = new ClientTextMessage;
                    cmsg->text = "game begins";
                    client_text_messages.push_back(cmsg);
                }
                break;
            }

            case GPCGameOver:
            {
                if (tournament) {
                    subsystem.play_system_sound(resources.get_sound("game_over"));
                    ClientTextMessage *cmsg = new ClientTextMessage;
                    cmsg->text = "GAME IS OVER";
                    client_text_messages.push_back(cmsg);
                }
                break;
            }

            case GPCPlayerChanged:
            {
                GPlayerDescription *pdesc = reinterpret_cast<GPlayerDescription *>(t->data);
                pdesc->from_net();
                for (Players::iterator it = players.begin(); it != players.end(); it++) {
                    Player *p = *it;
                    if (p->state.id == pdesc->id) {
                        std::string old_name = p->get_player_name();
                        std::string new_name(pdesc->player_name);
                        std::string old_skin(p->get_characterset()->get_name());
                        std::string new_skin(pdesc->characterset_name);

                        /* change player name */
                        if (old_name != new_name) {
                            p->set_player_name(new_name);
                            p->font = 0;
                            ClientTextMessage *cmsg = new ClientTextMessage;
                            cmsg->text = old_name + " is now known as " + new_name;
                            client_text_messages.push_back(cmsg);
                        }

                        /* change skin */
                        if (old_skin != new_skin) {
                            try {
                                p->set_characterset(new_skin);
                                ClientTextMessage *cmsg = new ClientTextMessage;
                                cmsg->text = p->get_player_name() + " changed the skin to " + new_skin;
                                client_text_messages.push_back(cmsg);
                            } catch (const Exception& e) {
                                subsystem << e.what() << std::endl;
                            }
                        }
                        break;
                    }
                }
                break;
            }

            case GPCScoreTransportRaw:
            {
                if (tournament) {
                    tournament->score_transport_raw(t->data);
                }
                break;
            }

            case GPCClanNames:
            {
                GClanNames *names = reinterpret_cast<GClanNames *>(t->data);
                names->from_net();
                team_red_name = names->red_name;
                team_blue_name = names->blue_name;
                if (tournament) {
                    tournament->set_team_names(team_red_name, team_blue_name);
                }
                break;
            }

            case GPCXferHeader:
            {
                reload_resources = true;
                GXferHeader *header = reinterpret_cast<GXferHeader *>(t->data);
                header->from_net();
                if (!fhnd) {
                    xfer_filename = header->filename;
                    current_download_filename = get_home_directory() + header->filename;
                    fhnd = fopen(current_download_filename.c_str(), "wb");
                    if (fhnd) {
                        total_xfer_sz = remaining_xfer_sz = header->filesize;
                    } else {
                        subsystem << "WARNING: cannot open file " << strerror(errno) << std::endl;
                    }
                } else {
                    subsystem << "WARNING: cannot receive " << xfer_filename << ". a file is already opened." << std::endl;
                }
                break;
            }

            case GPCXferDataChunk:
            {
                GXferDataChunk *chunk = reinterpret_cast<GXferDataChunk *>(t->data);
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
                    ScopeMutex lock(mtx);
                    send_data(evt.c, 0, GPSPakSyncAck, NetFlagsReliable, 0, 0);
                }
                break;
            }

            case GPCGenericData:
            {
                if (tournament) {
                    tournament->generic_data_delivery(t->data);
                }
                break;
            }

            case GPCServerQuit:
            {
                exception_msg.assign(reinterpret_cast<char *>(t->data), t->len);
                throw_exception = true;
                return;
                /* not necessary */
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
