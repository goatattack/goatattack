#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "Timing.hpp"

#include <string>
#include <cstring>
#ifdef __unix__
#include <arpa/inet.h>
#elif _WIN32
#include "Win.hpp"
#endif

/* IMPORTANT NOTE: an UDP packet should not exceed 576 bytes (RFC 791) */


/* increase, if protocol changes */
const int ProtocolVersion = 2;

/* --- */
typedef uint32_t hostaddr_t;
typedef uint16_t hostport_t;
typedef uint16_t pico_size_t;

typedef uint32_t sequence_no_t;
typedef unsigned char flags_t;
typedef unsigned char command_t;
typedef uint16_t data_len_t;
typedef uint8_t data_t;

const int PacketMaxSize = 500; /* safe packet size */

const int NetFlagsReliable = 1;

enum NetCommand {
    NetCommandStatReq = 0,
    NetCommandStatRsp,
    NetCommandWrongPassword,
    NetCommandServerFull,
    NetCommandAcknowledge,
    NetCommandPing,
    NetCommandPong,
    NetCommandLogin,
    NetCommandLogout,
    NetCommandData,
    _NetCommandMAX
};

#pragma pack(1)
struct NetMessage {
    NetMessage(sequence_no_t seq_no, flags_t flags, command_t cmd)
        : seq_no(seq_no), flags(flags), cmd(cmd) { }

    sequence_no_t seq_no;
    flags_t flags;
    command_t cmd;
    data_t data[1];     /* good old struct hack (must be 1, pedantic c++) */

    inline void from_net() {
        seq_no = ntohl(seq_no);
    }

    inline void to_net() {
        seq_no = htonl(seq_no);
    }
};
#pragma pack()

#pragma pack(1)
struct NetMessageData {
    NetMessageData() : len(0) { }
    NetMessageData(const std::string& var)
        : len(static_cast<data_len_t>(var.length()))
    {
        memcpy(data, var.c_str(), len);
    }
    data_len_t len;
    data_t data[1];

    inline void from_net() {
        len = ntohs(len);
    }

    inline void to_net() {
        len = htons(len);
    }
};
#pragma pack()

const int ServerStatusFlagNeedPassword = 1;

#pragma pack(1)
struct ServerStatusMsg {
    union {
        gametime_t ping;
        char padding[16];
    };
    pico_size_t max_heaps;
    pico_size_t cur_heaps;
    pico_size_t protocol_version;
    flags_t flags;
    data_len_t len;
    data_t name[1];

    inline void from_net() {
        max_heaps = ntohs(max_heaps);
        cur_heaps = ntohs(cur_heaps);
        protocol_version = ntohs(protocol_version);
        len = ntohs(len);
    }

    inline void to_net() {
        max_heaps = htons(max_heaps);
        cur_heaps = htons(cur_heaps);
        protocol_version = htons(protocol_version);
        len = htons(len);
    }
};
#pragma pack()

const int NetLoginPasswordLen = 16;

#pragma pack(1)
struct NetLogin {
    char pwd[NetLoginPasswordLen];
    data_len_t len;
    data_t data[1];

    inline void from_net() { }
    inline void to_net() { }
};
#pragma pack()

/* floating point must be IEEE 754 */
template<typename T> void byte_swap(T& data) {
    unsigned short v = 0x1234;
    uint8_t *pv = reinterpret_cast<uint8_t *>(&v);
    if (pv[0] == 0x34) {
        T temp = data;

        uint8_t *pdata = reinterpret_cast<uint8_t *>(&data);
        uint8_t *ptemp = reinterpret_cast<uint8_t *>(&temp);

        for (size_t i = 0; i < sizeof(data); i++) {
            pdata[i] = ptemp[(sizeof(data) - 1) - i];
        }
    }
}

#endif
