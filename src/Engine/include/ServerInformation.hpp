#ifndef SERVERINFORMATION_HPP
#define SERVERINFORMATION_HPP

#include "Protocol.hpp"

#include <string>
#include <vector>

enum ServerSort {
    Ascending = 0,
    Descending
};

class ServerInformation {
private:
    ServerInformation(const ServerInformation&);
    ServerInformation& operator=(const ServerInformation&);

public:
    ServerInformation();
    ServerInformation(hostaddr_t host, hostport_t port);
    virtual ~ServerInformation();

    hostaddr_t host;
    hostport_t port;
    std::string server_name;
    std::string map_name;
    pico_size_t max_clients;
    pico_size_t cur_clients;
    int protocol_version;
    bool secured;
    ms_t ping_time;
};

typedef std::vector<ServerInformation *> Hosts;

class ServerList {
private:
    ServerList(const ServerList&);
    ServerList& operator=(const ServerList&);

    typedef bool (*ServerSorter)(ServerInformation *lhs, ServerInformation *rhs);

public:
    ServerList();
    virtual ~ServerList();

    const Hosts& get_hosts() const;
    void sort(int index, ServerSort dir);
    void sort(int index);
    void sort();

private:
    int sort_index;
    ServerSort sort_dir;
    ServerSorter server_sorter;

protected:
    Hosts hosts;

private:
    ServerSorter get_server_sorter();
};

#endif
