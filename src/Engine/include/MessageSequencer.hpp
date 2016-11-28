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

#ifndef _MESSAGESEQUENCER_HPP_
#define _MESSAGESEQUENCER_HPP_

#include "Protocol.hpp"
#include "UDPSocket.hpp"
#include "Exception.hpp"
#include "I18N.hpp"

#include <deque>
#include <vector>
#include <time.h>

class MessageSequencerException : public Exception {
public:
    MessageSequencerException(const char *msg) : Exception(msg) { }
    MessageSequencerException(std::string msg) : Exception(msg) { }
};

struct Connection {
    Connection(hostaddr_t host, hostport_t port)
        : host(host), port(port), ping_time(0) { }

    hostaddr_t host;
    hostport_t port;
    ms_t ping_time;
};

struct QueueMessage;

struct SequencerHeap : public Connection {
    typedef std::deque<QueueMessage *> QueuedMessages;

    SequencerHeap(hostaddr_t host, hostport_t port)
        : Connection(host, port),
          sent_pings(0), active(true), deferred_kill(false),
          last_send_unrel_seq_no(0), last_send_rel_seq_no(0),
          last_recv_unrel_seq_no(0), last_recv_rel_seq_no(0)
    {
        last_ping.tv_sec = 0;
        last_ping.tv_nsec = 0;
    }

    int sent_pings;
    bool active;
    bool deferred_kill;

    sequence_no_t last_send_unrel_seq_no;
    sequence_no_t last_send_rel_seq_no;
    sequence_no_t last_recv_unrel_seq_no;
    sequence_no_t last_recv_rel_seq_no;

    gametime_t last_ping;

    QueuedMessages in_queue;
    QueuedMessages out_queue;
};

class MessageSequencer {
private:
    MessageSequencer(const MessageSequencer&);
    MessageSequencer& operator=(const MessageSequencer&);

public:
    enum LogoutReason {
        LogoutReasonRegular = 0,
        LogoutReasonPingTimeout,
        LogoutReasonTooManyResends,
        LogoutReasonApplicationQuit
    };

    enum RefusalReason {
        RefusalReasonServerFull = 0,
        RefusalReasonWrongPassword,
        RefusalReasonWrongProtocol
    };

    MessageSequencer(I18N& i18n, hostport_t port, pico_size_t num_heaps, const std::string& name, const std::string& password) throw (Exception);
    MessageSequencer(I18N& i18n, hostaddr_t server_host, hostport_t server_port) throw (Exception);
    virtual ~MessageSequencer();

    /* actions */
    void request_server_info(hostaddr_t host, hostport_t port) throw (Exception);
    void login(const std::string& password, data_len_t len, const void *data) throw (Exception);
    void login(data_len_t len, const void *data) throw (Exception);
    void logout() throw (Exception);

    void broadcast(flags_t flags, data_len_t len, const void *data) throw (Exception);
    void push(flags_t flags, data_len_t len, const void *data) throw (Exception);
    void push(flags_t flags, command_t cmd, data_len_t len, const void *data) throw (Exception);
    void push(const Connection *c, flags_t flags, data_len_t len, const void *data) throw (Exception);
    void push(const Connection *c, command_t cmd, flags_t flags, data_len_t len, const void *data) throw (Exception);
    size_t get_outq_sz(const Connection *c);
    size_t get_inq_sz(const Connection *c);
    bool cycle() throw (Exception);
    void kill(const Connection *c) throw (Exception);
    void new_settings(hostport_t port, pico_size_t num_heaps, const std::string& name,
        const std::string& password) throw (Exception);
    const SequencerHeap *get_heap(const Connection *c) const;

    /* events */
    virtual void event_status(hostaddr_t host, hostport_t port, const std::string& name,
        int max_clients, int cur_clients, ms_t ping_time, bool secured,
        int protocol_version) throw (Exception) { }
    virtual void event_access_denied(RefusalReason reason) throw (Exception) { }
    virtual void event_login(const Connection *c, data_len_t len, void *data) throw (Exception) { }
    virtual void event_data(const Connection *c, data_len_t len, void *data) throw (Exception) { }
    virtual void event_logout(const Connection *c, LogoutReason reason) throw (Exception) { }

protected:
    I18N& i18n;

private:
    typedef std::vector<SequencerHeap *> SequencerHeaps;

    pico_size_t max_heaps;
    bool is_client;
    std::string name;
    std::string password;
    UDPSocket socket;
    NetMessage *pmsg;
    NetMessageData *pdata;

    SequencerHeaps heaps;
    char buffer[1024];

    void ack(SequencerHeap *heap, sequence_no_t seq_no) throw (Exception);
    void process_incoming(SequencerHeap *heap, NetMessage *msg) throw (Exception);
    void sorted_insert(SequencerHeap::QueuedMessages& msg_queue, QueueMessage *smsg);
    SequencerHeap *find_heap(hostaddr_t host, hostport_t port);
    SequencerHeap *find_heap(const Connection *c);
    void delete_heap(SequencerHeap *heap);
    void kill_heap_with_logout(SequencerHeap *heap, LogoutReason reason);
    void flush_queues(SequencerHeap *heap);
    void slack_send(hostaddr_t host, hostport_t port, sequence_no_t seq_no,
        flags_t flags, command_t cmd, data_len_t len, const void *data) throw (Exception);
    void delete_all_heaps();
};

#endif
