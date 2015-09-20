#ifndef MASTERQUERYCLIENT_HPP
#define MASTERQUERYCLIENT_HPP

#include "MessageSequencer.hpp"
#include "ServerInformation.hpp"

class MasterQuery;

class MasterQueryClient : public ServerInformation, public MessageSequencer {
public:
    MasterQueryClient(MasterQuery& master_query, hostaddr_t host, hostport_t port) throw (Exception);
    virtual ~MasterQueryClient();

    MasterQuery& master_query;
    bool received;

    gametime_t last_update;

    void refresh();
    void timed_refresh(gametime_t& now);

    virtual void event_status(hostaddr_t host, hostport_t port, const std::string& name,
        int max_clients, int cur_clients, ms_t ping_time, bool secured,
        int protocol_version) throw (Exception);
};

#endif // MASTERQUERYCLIENT_HPP
