#include "MasterQueryClient.hpp"
#include "Timing.hpp"
#include "MasterQuery.hpp"

MasterQueryClient::MasterQueryClient(MasterQuery& master_query, hostaddr_t host, hostport_t port) throw (Exception)
    : GameserverInformation(host, port), MessageSequencer(host, port), master_query(master_query), received(false)
{
    get_now(last_update);
}

MasterQueryClient::~MasterQueryClient() { }

void MasterQueryClient::refresh() {
    received = false;
    request_server_info(host, port);
}

void MasterQueryClient::timed_refresh(gametime_t& now) {
    if (!received) {
        if (diff_ms(last_update, now) > 250) {
            refresh();
            last_update = now;
        }
    }
}

void MasterQueryClient::event_status(hostaddr_t host, hostport_t port, const std::string& name,
    int max_clients, int cur_clients, ms_t ping_time, bool secured,
    int protocol_version) throw (Exception)
{
    this->server_name = name;
    this->map_name = "";
    this->max_clients = max_clients;
    this->cur_clients = cur_clients;
    this->ping_time = ping_time;
    this->secured = secured;
    this->protocol_version = protocol_version;

    received = true;

    master_query.locked_sort();
}
