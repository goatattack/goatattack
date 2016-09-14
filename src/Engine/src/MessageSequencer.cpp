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

#include "MessageSequencer.hpp"
#include "AutoPtr.hpp"

#include <cstdlib>
#include <cstdio>

/* to simulate crappy net links:
 *
 * # tc qdisc add dev lo root netem delay 150ms 10ms 25%
 * # tc qdisc del dev lo root
 */

/* some consts to tweak */
static const int PingTimeout = 5000; /* 5 seconds */
static const int PingInterval = 500; /* 500 ms    */
static const int MaxResends = 7;     /* 6250 ms   */

/* protocol v4 changed status request structure */
static const int NewStatusProtocolVersion = 4;

/* subtract 1 of the name[1] -> c++ forbids zero arrays eg. name[0] */
static const int MsgHeaderLength = sizeof(NetMessage) - 1 + sizeof(NetMessageData) - 1;
static const int ServerStatusLength = sizeof(ServerStatusMsg) - 1;

ScopeHeapMarker::ScopeHeapMarker(SequencerHeap& heap) : heap(heap) {
    heap.processing = true;
}

ScopeHeapMarker::~ScopeHeapMarker() {
    heap.processing = false;
}

MessageSequencer::MessageSequencer(hostport_t port, pico_size_t max_heaps,
    const std::string& name, const std::string& password) throw (Exception)
    : max_heaps(max_heaps), is_client(false), name(name), password(password), socket(port),
      pmsg(reinterpret_cast<NetMessage *>(buffer)),
      pdata(reinterpret_cast<NetMessageData *>(pmsg->data))
{
    memset(buffer, 0, sizeof buffer);
}

MessageSequencer::MessageSequencer(hostaddr_t server_host, hostport_t server_port)
    throw (Exception)
    : max_heaps(1), is_client(true), name(), password(), socket(),
      pmsg(reinterpret_cast<NetMessage *>(buffer)),
      pdata(reinterpret_cast<NetMessageData *>(pmsg->data))
{
    memset(buffer, 0, sizeof buffer);

    SequencerHeap *heap = new SequencerHeap(server_host, server_port);
    heaps.push_back(heap);

    request_server_info(server_host, server_port);
}

MessageSequencer::~MessageSequencer() {
    delete_all_heaps();
}

void MessageSequencer::request_server_info(hostaddr_t host, hostport_t port) throw (Exception) {
    ServerStatusMsg stat;
    memset(&stat, 0, sizeof(stat));
    get_now(stat.ping);
    slack_send(host, port, 0, 0, NetCommandStatReq, ServerStatusLength, &stat);
}

void MessageSequencer::login(const std::string& password, data_len_t len, const void *data) throw (Exception) {
    int sz = sizeof(NetLogin) + len;
    AutoPtr<char []> tmp(new char[sz]);
    NetLogin *login = reinterpret_cast<NetLogin *>(&tmp[0]);
    memset(login, 0, sizeof(NetLogin));
    login->protocol_version = ProtocolVersion;
    strncpy(login->pwd, password.c_str(), NetLoginPasswordLen - 1);
    login->len = len;
    if (len) {
        memcpy(login->data, data, len);
    }
    login->to_net();
    push(NetFlagsReliable, NetCommandLogin, sz, login);
}

void MessageSequencer::login(data_len_t len, const void *data) throw (Exception) {
    login("", len, data);
}

void MessageSequencer::logout() throw (Exception) {
    push(NetFlagsReliable, NetCommandLogout, 0, 0);
}

void MessageSequencer::broadcast(flags_t flags, data_len_t len, const void *data) throw (Exception) {
    for (SequencerHeaps::iterator it = heaps.begin(); it != heaps.end(); it++) {
        SequencerHeap *heap = *it;
        push(heap, flags, len, data);
    }
}

void MessageSequencer::push(flags_t flags, data_len_t len, const void *data) throw (Exception) {
    if (heaps.size()) {
        push(heaps[0], flags, len, data);
    }
}

void MessageSequencer::push(flags_t flags, command_t cmd, data_len_t len,
    const void *data) throw (Exception)
{
    if (heaps.size()) {
        push(heaps[0], cmd, flags, len, data);
    }
}

void MessageSequencer::push(const Connection *c, flags_t flags, data_len_t len,
    const void *data) throw (Exception)
{
    push(c, NetCommandData, flags, len, data);
}

void MessageSequencer::push(const Connection *c, command_t cmd, flags_t flags,
    data_len_t len, const void *data) throw (Exception)
{
    SequencerHeap *h = find_heap(c);
    if (h && h->active) {
        if (flags & NetFlagsReliable) {
            data_t *alloc_data = 0;
            if (len) {
                alloc_data = new data_t[len];
                memcpy(alloc_data, data, len);
            }
            QueueMessage *m = new QueueMessage(++h->last_send_rel_seq_no, flags,
                cmd, len, alloc_data);
            h->out_queue.push_back(m);
        } else {
            slack_send(h->host, h->port, ++h->last_send_unrel_seq_no,
                flags, cmd, len, data);
        }
    }
}

bool MessageSequencer::cycle() throw (Exception) {
    hostaddr_t host;
    hostport_t port;
    bool recycle;
    bool again = false;
    gametime_t touch;

    /* get current ticks */
    get_now(touch);

    /* incoming data? */
    ssize_t sz = socket.recv(buffer, sizeof(buffer), &host, &port);
    if (sz) {
        again = true;
        pmsg->from_net();
        pdata->from_net();

        if (pmsg->cmd == NetCommandStatReq) {
            /* server status request */
            if (!is_client && max_heaps) {
                ServerStatusMsg *stat = reinterpret_cast<ServerStatusMsg *>(pdata->data);
                stat->protocol_version = ProtocolVersion;
                stat->max_heaps = max_heaps;
                stat->cur_heaps = static_cast<pico_size_t>(heaps.size());
                stat->flags = (password.length() ? ServerStatusFlagNeedPassword : 0);
                stat->len = static_cast<data_len_t>(name.length());
                int len = stat->len;
                memcpy(stat->name, name.c_str(), stat->len);
                stat->to_net();
                slack_send(host, port, 0, 0, NetCommandStatRsp, pdata->len + len, pdata->data);
            }
        } else if (pmsg->cmd == NetCommandStatRsp) {
            /* server status event */
            if (is_client || !max_heaps) {
                ServerStatusMsg *stat = reinterpret_cast<ServerStatusMsg *>(pdata->data);
                stat->from_net();
                ms_t ping_time = diff_ms(stat->ping, touch);
                bool secured = ((stat->flags & ServerStatusFlagNeedPassword) != 0);
                const char *payload = "---[ server too old ]---";
                if (stat->protocol_version >= NewStatusProtocolVersion) {
                    stat->name[stat->len] = 0;
                    payload = reinterpret_cast<const char *>(stat->name);
                }
                event_status(host, port, payload, stat->max_heaps, stat->cur_heaps, ping_time, secured, stat->protocol_version);
            }
        } else if (pmsg->cmd == NetCommandServerFull) {
            if (is_client) {
                event_access_denied(RefusalReasonServerFull);
                delete_all_heaps();
            }
        } else if (pmsg->cmd == NetCommandWrongPassword) {
            if (is_client) {
                event_access_denied(RefusalReasonWrongPassword);
                delete_all_heaps();
            }
        } else if (pmsg->cmd == NetCommandWrongProtocol) {
            if (is_client) {
                event_access_denied(RefusalReasonWrongProtocol);
                delete_all_heaps();
            }
        } else {
            /* login attempt, create new heap */
            SequencerHeap *h = 0;
            if (pmsg->cmd == NetCommandLogin) {
                if (!is_client && !find_heap(host, port)) {
                    NetLogin *login = reinterpret_cast<NetLogin *>(pdata->data);
                    login->from_net();
                    std::string pwd(login->pwd);
                    if (login->protocol_version != ProtocolVersion) {
                        slack_send(host, port, 0, 0, NetCommandWrongProtocol, 0, 0);
                    } else if (pwd != password) {
                        slack_send(host, port, 0, 0, NetCommandWrongPassword, 0, 0);
                    } else if (heaps.size() < max_heaps) {
                        h = new SequencerHeap(host, port);
                        heaps.push_back(h);
                    } else {
                        slack_send(host, port, 0, 0, NetCommandServerFull, 0, 0);
                    }
                }
            } else {
                h = find_heap(host, port);
            }

            /* process, if valid heap */
            if (h) {
                if (pmsg->flags & NetFlagsReliable) {
                    /* queueing */
                    if (pmsg->seq_no > h->last_recv_rel_seq_no) {
                        data_t *alloc_data = 0;
                        if (pdata->len) {
                            alloc_data = new data_t[pdata->len];
                            memcpy(alloc_data, pdata->data, pdata->len);
                        }
                        QueueMessage *m = new QueueMessage(pmsg->seq_no, pmsg->flags,
                            pmsg->cmd, pdata->len, alloc_data);
                        sorted_insert(h->in_queue, m);
                    } else {
                        ack(h, pmsg->seq_no);
                    }
                } else {
                    if (pmsg->seq_no > h->last_recv_unrel_seq_no) {
                        h->last_recv_unrel_seq_no = pmsg->seq_no;
                        process_incoming(h, pmsg);
                    }
                }
            }
        }
    }

    /* check all heaps */
    do {
        recycle = false;
        for (SequencerHeaps::iterator it = heaps.begin(); it != heaps.end(); it++) {
            SequencerHeap *h = *it;

            /* inspect incoming queue */
            if (h->in_queue.size()) {
                QueueMessage *tmp_smsg = h->in_queue[0];
                if (tmp_smsg->seq_no == h->last_recv_rel_seq_no + 1) {
                    h->last_recv_rel_seq_no = tmp_smsg->seq_no;
                    pmsg->seq_no = tmp_smsg->seq_no;
                    pmsg->flags = tmp_smsg->flags;
                    pmsg->cmd = tmp_smsg->cmd;
                    pdata->len = tmp_smsg->len;
                    memcpy(pdata->data, tmp_smsg->data, tmp_smsg->len);
                    process_incoming(h, pmsg);
                    h->in_queue.pop_front();
                    ack(h, tmp_smsg->seq_no);
                    command_t tmp_cmd = tmp_smsg->cmd;
                    delete tmp_smsg;
                    if (h->deferred_kill) {
                        kill_heap_with_logout(h, LogoutReasonApplicationQuit);
                        recycle = true;
                        break;
                    } else if (tmp_cmd == NetCommandLogout) {
                        delete_heap(h);
                        recycle = true;
                        break;
                    }
                }
            }

            /* re/-send deepest entry in outgoing queue */
            if (h->out_queue.size()) {
                QueueMessage *tmp_smsg = h->out_queue[0];
                ms_t diff = diff_ms(tmp_smsg->touch, touch);
                if (!tmp_smsg->touch.tv_sec || diff >= tmp_smsg->last_resend_ms) {
                    again = true;
                    tmp_smsg->touch = touch;
                    tmp_smsg->resends++;
                    tmp_smsg->last_resend_ms *= 2;
                    if (tmp_smsg->resends > MaxResends) {
                        /* disconnect after too many resends */
                        kill_heap_with_logout(h, LogoutReasonTooManyResends);
                        recycle = true;
                        break;
                    } else {
                        slack_send(h->host, h->port, tmp_smsg->seq_no, tmp_smsg->flags,
                            tmp_smsg->cmd, tmp_smsg->len, tmp_smsg->data);
                    }
                }
            }

            /* send unrel ping */
            ms_t diff = diff_ms(h->last_ping, touch);
            if (!h->last_ping.tv_sec || diff >= PingInterval) {
                get_now(h->last_ping);
                push(h, NetCommandPing, 0, sizeof(gametime_t), &h->last_ping);
                h->sent_pings++;
            }
            if (h->sent_pings > PingTimeout / PingInterval) {
                /* after x seconds of absolutely silence, disconnect client */
                kill_heap_with_logout(h, LogoutReasonPingTimeout);
                recycle = true;
                break;
            }
        }
    } while (recycle);

    /* again? */
    return again;
}

void MessageSequencer::kill(const Connection *c) throw (Exception) {
    SequencerHeap *h = static_cast<SequencerHeap *>(const_cast<Connection *>(c));
    if (h->processing) {
        h->deferred_kill = true;
    } else {
        kill_heap_with_logout(h, LogoutReasonApplicationQuit);
    }
}

void MessageSequencer::new_settings(hostport_t port, pico_size_t num_heaps,
    const std::string& name, const std::string& password) throw (Exception)
{
    int current = static_cast<int>(heaps.size());
    if (current >= num_heaps) {
        char temp_buffer[256];
        sprintf(temp_buffer, "Cannot set num_heaps to %d, %d are logged in", num_heaps, current);
        throw MessageSequencerException(temp_buffer);
    }
    this->name = name;
    this->password = password;
    this->max_heaps = num_heaps;
    socket.set_port(port);
}

void MessageSequencer::ack(SequencerHeap *heap, sequence_no_t seq_no) throw (Exception) {
    sequence_no_t net_seq_no = htonl(seq_no);
    slack_send(heap->host, heap->port, ++heap->last_send_unrel_seq_no,
        0, NetCommandAcknowledge, sizeof(sequence_no_t), &net_seq_no);
}

void MessageSequencer::process_incoming(SequencerHeap *heap, NetMessage *msg) throw (Exception) {
    ScopeHeapMarker heap_marker(*heap);

    NetMessageData *data = reinterpret_cast<NetMessageData *>(msg->data);

    switch (msg->cmd) {
        case NetCommandAcknowledge:
        {
            if (heap->out_queue.size()) {
                sequence_no_t *pseq_no = reinterpret_cast<sequence_no_t *>(data->data);
                sequence_no_t seq_no = ntohl(*pseq_no);
                QueueMessage *tmp_smsg = heap->out_queue[0];
                if (tmp_smsg->seq_no == seq_no) {
                    if (tmp_smsg->cmd == NetCommandLogin) {
                        event_login(heap, tmp_smsg->len, tmp_smsg->data);
                    }
                    delete tmp_smsg;
                    heap->out_queue.pop_front();
                }
            }
            break;
        }

        case NetCommandPing:
        {
            push(heap, NetCommandPong, 0, data->len, data->data);
            break;
        }

        case NetCommandPong:
        {
            gametime_t *pseq_time = reinterpret_cast<gametime_t *>(data->data);
            gametime_t now;
            get_now(now);
            heap->ping_time = diff_ms(*pseq_time, now);
            heap->sent_pings = 0;
            break;
        }

        case NetCommandLogin:
        {
            if (!is_client) {
                NetLogin *login = reinterpret_cast<NetLogin *>(pdata->data);
                login->data[login->len] = 0;
                event_login(heap, login->len, login->data);
            }
            break;
        }

        case NetCommandLogout:
        {
            if (!is_client) {
                event_logout(heap, LogoutReasonRegular);
            }
            break;
        }

        case NetCommandData:
        {
            data->data[data->len] = 0;
            event_data(heap, data->len, data->data);
            break;
        }
    }
}

void MessageSequencer::sorted_insert(SequencerHeap::QueuedMessages& msg_queue,
    QueueMessage *smsg)
{
    SequencerHeap::QueuedMessages::iterator it;
    for (it = msg_queue.begin(); it != msg_queue.end(); it++) {
        QueueMessage *tmp_smsg = *it;
        if (tmp_smsg->seq_no > smsg->seq_no) {
            break;
        }
    }
    msg_queue.insert(it, smsg);
}

SequencerHeap *MessageSequencer::find_heap(hostaddr_t host, hostport_t port) {
    for (SequencerHeaps::iterator it = heaps.begin(); it != heaps.end(); it++) {
        SequencerHeap *heap = *it;
        if (heap->host == host && heap->port == port) {
            return heap;
        }
    }

    return 0;
}

SequencerHeap *MessageSequencer::find_heap(const Connection *c) {
    for (SequencerHeaps::iterator it = heaps.begin(); it != heaps.end(); it++) {
        if (*it == c) {
            return static_cast<SequencerHeap *>(const_cast<Connection *>(c));
        }
    }

    return 0;
}

void MessageSequencer::delete_heap(SequencerHeap *heap) {
    for (SequencerHeaps::iterator it = heaps.begin(); it != heaps.end(); it++) {
        SequencerHeap *h = *it;
        if (h == heap) {
            flush_queues(heap);
            delete heap;
            heaps.erase(it);
            break;
        }
    }
}

void MessageSequencer::kill_heap_with_logout(SequencerHeap *heap, LogoutReason reason) {
    heap->active = false;
    event_logout(heap, reason);
    delete_heap(heap);
}

void MessageSequencer::flush_queues(SequencerHeap *heap) {
    SequencerHeap::QueuedMessages::iterator it;

    /* purge in_queue */
    for (it = heap->in_queue.begin(); it != heap->in_queue.end(); it++) {
        delete *it;
    }
    heap->in_queue.clear();

    /* purge out_queue */
    for (it = heap->out_queue.begin(); it != heap->out_queue.end(); it++) {
        delete *it;
    }
    heap->out_queue.clear();
}

void MessageSequencer::slack_send(hostaddr_t host, hostport_t port,
    sequence_no_t seq_no, flags_t flags, command_t cmd, data_len_t len,
    const void *data) throw (Exception)
{
    pmsg->seq_no = seq_no;
    pmsg->flags = flags;
    pmsg->cmd = cmd;
    pmsg->to_net();

    pdata->len = len;
    pdata->to_net();
    if (len && pdata->data != data) {
        memcpy(pdata->data, data, len);
    }

    socket.send(host, port, pmsg, MsgHeaderLength + len);
}

void MessageSequencer::delete_all_heaps() {
    while (heaps.size()) {
        flush_queues(heaps[0]);
        delete heaps[0];
        heaps.erase(heaps.begin());
    }
}
