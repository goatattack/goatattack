#include "LANBroadcaster.hpp"
#include "Scope.hpp"

LANBroadcaster::LANBroadcaster(hostport_t port) throw (LANBroadcasterException, UDPSocketException)
    : MessageSequencer(port, 0, "", ""), port(port), running(false)
{
    refresh();
    start();
}

LANBroadcaster::~LANBroadcaster() {
    stop();
    cleanup();
}

void LANBroadcaster::start() throw (LANBroadcasterException) {
    if (!running) {
        running = true;
        if (!thread_start()) {
            throw LANBroadcasterException("Starting thread failed.");
        }
    }
}

void LANBroadcaster::stop() {
    if (running) {
        running = false;
        thread_join();
    }
}

void LANBroadcaster::refresh() throw (Exception) {
    request_server_info(INADDR_BROADCAST, port);
}

void LANBroadcaster::clear() {
    cleanup();
}

Mutex& LANBroadcaster::get_mutex() {
    return mtx;
}

void LANBroadcaster::event_status(hostaddr_t host, hostport_t port,
    const std::string& name, int max_clients, int cur_clients, ms_t ping_time,
    bool secured, int protocol_version) throw (Exception)
{
    GameserverInformation *info = 0;
    bool found = false;

    /* find in list */
    for (Hosts::iterator it = hosts.begin(); it != hosts.end(); it++) {
        info = *it;
        if (info->host == host && info->port == port) {
            found = true;
            break;
        }
    }

    /* add/update information */
    if (!found) {
        info = new GameserverInformation;
        hosts.push_back(info);
    }
    info->server_name = name;
    info->host = host;
    info->port = port;
    info->max_clients = max_clients;
    info->cur_clients = cur_clients;
    info->ping_time = ping_time;
    info->secured = secured;
    info->protocol_version = protocol_version;

    sort();
}

void LANBroadcaster::cleanup() {
    for (Hosts::iterator it = hosts.begin(); it != hosts.end(); it++) {
        delete *it;
    }
    hosts.clear();
}

void LANBroadcaster::thread() {
    while (running) {
        {
            Scope<Mutex> lock(mtx);
            while(cycle());
        }
        wait_ns(1000000);
    }
}
