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


#ifndef _GAMEPROTOCOL_HPP_
#define _GAMEPROTOCOL_HPP_

#include "Protocol.hpp"

typedef int32_t pos_t;
typedef pico_size_t player_id_t;
typedef unsigned char gamestate_t;
typedef unsigned char joyaxis_t;
typedef uint16_t keystates_t;
typedef int16_t joyvalue_t;
typedef int16_t scounter_t;
typedef int16_t identifier_t;
typedef unsigned char transflag_t;
typedef unsigned char nano_size_t;
typedef uint16_t score_t;
typedef int16_t sscore_t;
typedef uint16_t playerflags_t;
typedef uint32_t sr_milliseconds_t;
typedef uint32_t datasize_t;

const int NameLength = 32;
const int TextLength = 64;
const int FilenameLen = 64;

const int PlayerKeyStateLeft = 1;
const int PlayerKeyStateRight = 2;
const int PlayerKeyStateUp = 4;
const int PlayerKeyStateDown = 8;
const int PlayerKeyStateJump = 16;
const int PlayerKeyStateFire = 32;
const int PlayerKeyStateDrop1 = 64;
const int PlayerKeyStateDrop2 = 128;
const int PlayerKeyStateDrop3 = 256;


/* current game state */
enum ServerState {
    ServerStateSelectTournament = 0,
    ServerStatePlay,
    ServerStateOver
};

/* game protocol server to client */
enum GPC {
    GPCServerMessage = 0,
    GPCMapState,
    GPCIdentifyPlayer,
    GPCReady,
    GPCAddPlayer,
    GPCRemovePlayer,
    GPCResetPlayer,
    GPCSpawnPlayer,
    GPCUpdateGameState,
    GPCUpdatePlayerState,
    GPCUpdateObject,
    GPCUpdateAnimation,
    GPCUpdateSpawnableNPC,
    GPCSpawnNPC,
    GPCRemoveNPC,
    GPCTextMessage,
    GPCChatMessage,
    GPCAddAnimation,
    GPCAddTextAnimation,
    GPCPlayerRecoil,
    GPCPlayerHurt,
    GPCPickObject,
    GPCSpawnObject,
    GPCPlaceObject,
    GPCJoinAccepted,
    GPCJoinRefused,
    GPCTeamScore,
    GPCFriendlyFire,
    GPCGamePlayUnbalanced,
    GPCTimeRemaining,
    GPCWarmUp,
    GPCGameBegins,
    GPCGameOver,
    GPCPlayerChanged,
    GPCPlaySound,
    GPCScoreTransportRaw,
    GPCRemoveAnimation,
    GPCXferHeader,
    GPCXferDataChunk,
    GPCGenericData,
    GPCPakHash,
    GPCServerQuit,
    GPCSpectate,
    GPCI18NText,
    GPCTournamentSetting
};

/* game protocol client to server */
enum GPS {
    GPSUpdatePlayerClientServerState = 0,
    GPSRespawnRequest,
    GPSJoinRequest,
    GPSChatMessage,
    GPSShot,
    GPSGrenade,
    GPSBomb,
    GPSFrog,
    GPSPlayerChanged,
    GPSRoundFinished,
    GPSPakSyncHash,
    GPSPakSyncHashFinished,
    GPSPakSyncAck,
    GPSSpectate
};

const int TransportFlagMorePackets = 1;

// OK
#pragma pack(1)
struct GTransport {
    command_t cmd;                  // 1
    transflag_t flags;              // 1
    unsigned char tournament_id;    // 1
    data_len_t len;                 // 2
    data_t data[1];                 // 1

    inline void from_net() {
        len = ntohs(len);
    }

    inline void to_net() {
        len = htons(len);
    }
};
#pragma pack()

const int TournamentFlagWarmup = 1;

// OK
#pragma pack(1)
struct GTournament {
    char map_name[NameLength];      // 32
    unsigned char gametype;         // 1
    unsigned char tournament_id;    // 1
    scounter_t duration;            // 2
    scounter_t warmup;              // 2
    flags_t flags;                  // 1

    inline void from_net() {
        duration = ntohs(duration);
        warmup = ntohs(warmup);
    }

    inline void to_net() {
        duration = htons(duration);
        warmup = htons(warmup);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GTournamentSetting {
    unsigned char setting_id;       // 1
    flags_t flag;                   // 1
};
#pragma pack()

// OK
#pragma pack(1)
struct GGameState {
    GGameState() : seconds_remaining(0) { }
    GGameState(scounter_t seconds_remaining)
        : seconds_remaining(seconds_remaining) { }
    scounter_t seconds_remaining;

    inline void from_net() {
        seconds_remaining = ntohs(seconds_remaining);
    }

    inline void to_net() {
        seconds_remaining = htons(seconds_remaining);
    }
};
#pragma pack()

const int PlayerClientFlagFalling = 1;
const int PlayerClientFlagOneShotFinished = 2;
const int PlayerClientFlagLanded = 4;
const int PlayerClientFlagJumpReleased = 8;
const int PlayerClientFlagFireReleased = 16;
const int PlayerClientFlagGrenadeReleased = 32;
const int PlayerClientFlagBombReleased = 64;
const int PlayerClientFlagFrogReleased = 128;
const int PlayerClientFlagDoShotAnimation = 256;

// OK
#pragma pack(1)
struct GPlayerClientState {
    playerflags_t flags;         // 2
    unsigned char icon;          // 1
    unsigned char iconindex;     // 1

    inline void from_net() {
        flags = ntohs(flags);
    }

    inline void to_net() {
        flags = htons(flags);
    }
};
#pragma pack()

const int PlayerServerFlagSpectating = 1;
const int PlayerServerFlagDead = 2;
const int PlayerServerFlagTeamRed = 4;
const int PlayerServerFlagHasOppositeFlag = 8;
const int PlayerServerFlagHasShotgunBelt = 16;
const int PlayerServerFlagHasCoin = 32;

// OK (rearranged)
#pragma pack(1)
struct GPlayerServerState {
    pico_size_t ping_time;       // 2
    sscore_t score;              // 2
    score_t frags;               // 2
    score_t kills;               // 2
    nano_size_t outq_sz;         // 1
    playerflags_t flags;         // 1
    unsigned char health;        // 1
    unsigned char ammo;          // 1
    unsigned char armor;         // 1
    unsigned char bombs;         // 1
    unsigned char grenades;      // 1
    unsigned char frogs;         // 1

    inline void from_net() {
        flags = ntohs(flags);
        ping_time = ntohs(ping_time);
        score = ntohs(score);
        frags = ntohs(frags);
        kills = ntohs(kills);
    }

    inline void to_net() {
        flags = htons(flags);
        ping_time = htons(ping_time);
        score = htons(score);
        frags = htons(frags);
        kills = htons(kills);
    }
};
#pragma pack()

const int PlayerClientServerFlagWriting = 1;
const int PlayerClientServerFlagForceBroadcast = 2;

// OK (rearranged)
#pragma pack(1)
struct GPlayerClientServerState {
    double x;                    // 8
    double y;                    // 8
    double accel_x;              // 8
    double accel_y;              // 8
    double jump_accel_y;         // 8
    keystates_t key_states;      // 2
    playerflags_t flags;         // 2
    joyaxis_t jaxis;             // 1
    unsigned char direction;     // 1


    inline void from_net() {
        key_states = htons(key_states);
        flags = ntohs(flags);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
        byte_swap<double>(jump_accel_y);
    }

    inline void to_net() {
        key_states = ntohs(key_states);
        flags = htons(flags);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
        byte_swap<double>(jump_accel_y);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GPlayerState {
    player_id_t id;              // 2
    GPlayerClientState client_state;
    GPlayerClientServerState client_server_state;
    GPlayerServerState server_state;

    inline void from_net() {
        id = ntohs(id);
        client_state.from_net();
        client_server_state.from_net();
        server_state.from_net();
    }

    inline void to_net() {
        id = htons(id);
        client_state.to_net();
        client_server_state.to_net();
        server_state.to_net();
    }
};
#pragma pack()

/* game player transport helper structs with id */
// OK
#pragma pack(1)
struct GPTAllStates {
    identifier_t id;
    GPlayerServerState server_state;
    GPlayerClientServerState client_server_state;

    inline void from_net() {
        id = ntohs(id);
        server_state.from_net();
        client_server_state.from_net();
    }

    inline void to_net() {
        id = htons(id);
        server_state.to_net();
        client_server_state.to_net();
    }
};
#pragma pack()

/* other structs */
// OK
#pragma pack(1)
struct GPlayerDescription {
    player_id_t id;
    char player_name[NameLength];
    char characterset_name[NameLength];

    inline void from_net() {
        id = ntohs(id);
    }

    inline void to_net() {
        id = htons(id);
    }
};
#pragma pack()

#pragma pack(1)
struct GGenericName {
    char name[NameLength];
};
#pragma pack()

// OK
#pragma pack(1)
struct GPlayerInfo {
    player_id_t id;
    GPlayerDescription desc;
    GPlayerClientServerState client_server_state;
    GPlayerServerState server_state;
    GPlayerClientState client_state;

    inline void from_net() {
        id = ntohs(id);
        client_server_state.from_net();
        server_state.from_net();
        client_state.from_net();
    }

    inline void to_net() {
        id = htons(id);
        client_server_state.to_net();
        server_state.to_net();
        client_state.to_net();
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GAnimation {
    char animation_name[NameLength];
    char sound_name[NameLength];
    double x;                   // 8
    double y;                   // 8
    double accel_x;             // 8
    double accel_y;             // 8
    identifier_t id;            // 2
    scounter_t duration;        // 2
    identifier_t owner;         // 2
    inline void from_net() {
        id = ntohs(id);
        duration = ntohs(duration);
        owner = ntohs(owner);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }

    inline void to_net() {
        id = htons(id);
        duration = htons(duration);
        owner = htons(owner);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }
};
#pragma pack()

const int GTextAnimationFlagCenterScreen = 1;

// OK
#pragma pack(1)
struct GTextAnimation {
    char font_name[NameLength];
    double x;                           // 8
    double y;                           // 8
    scounter_t max_counter;             // 2
    identifier_t i18n_id;               // 2
    flags_t flags;                      // 1

    inline void from_net() {
        byte_swap<double>(x);
        byte_swap<double>(y);
        max_counter = ntohs(max_counter);
        i18n_id = ntohs(i18n_id);
    }

    inline void to_net() {
        byte_swap<double>(x);
        byte_swap<double>(y);
        max_counter = htons(max_counter);
        i18n_id = htons(i18n_id);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GPickObject {
    identifier_t id;                    // 2

    inline void from_net() {
        id = ntohs(id);
    }

    inline void to_net() {
        id = htons(id);
    }
};
#pragma pack()

const int PlaceObjectWithAnimation = 1;
const int PlaceObjectWithSpawnSound = 2;
const int PlaceObjectWithScoredSound = 4;
const int PlaceObjectWithDropSound = 8;
const int PlaceObjectResetVelocity = 16;

// OK (rearranged)
#pragma pack(1)
struct GPlaceObject {
    pos_t x;                    // 4
    pos_t y;                    // 4
    identifier_t id;            // 2
    flags_t flags;              // 1

    inline void from_net() {
        id = ntohs(id);
        x = ntohl(x);
        y = ntohl(y);
    }

    inline void to_net() {
        id = htons(id);
        x = htonl(x);
        y = htonl(y);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GSpawnObject {
    char object_name[NameLength];
    pos_t x;                    // 4
    pos_t y;                    // 4
    identifier_t id;            // 2
    flags_t flags;              // 1

    inline void from_net() {
        id = ntohs(id);
        x = ntohl(x);
        y = ntohl(y);
    }

    inline void to_net() {
        id = htons(id);
        x = htonl(x);
        y = htonl(y);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GObjectState {
    double x;                   // 8
    double y;                   // 8
    double accel_x;             // 8
    double accel_y;             // 8
    identifier_t id;            // 2

    inline void from_net() {
        id = ntohs(id);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }

    inline void to_net() {
        id = htons(id);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GSpawnNPC {
    char npc_name[NameLength];
    char sound_name[NameLength];
    double x;                           // 8
    double y;                           // 8
    double accel_x;                     // 8
    double accel_y;                     // 8
    identifier_t id;                    // 2
    identifier_t owner;                 // 2
    unsigned char direction;            // 1
    unsigned char icon;                 // 1

    inline void from_net() {
        id = ntohs(id);
        owner = ntohs(owner);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }

    inline void to_net() {
        id = htons(id);
        owner = htons(owner);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GRemoveNPC {
    identifier_t id;                // 2

    inline void from_net() {
        id = ntohs(id);
    }

    inline void to_net() {
        id = htons(id);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GNPCState {
    double x;                       // 8
    double y;                       // 8
    double accel_x;                 // 8
    double accel_y;                 // 8
    identifier_t id;                // 2
    identifier_t owner;             // 2
    playerflags_t flags;            // 2
    unsigned char direction;        // 1

    inline void from_net() {
        id = ntohs(id);
        owner = ntohs(owner);
        flags = ntohs(flags);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }

    inline void to_net() {
        id = htons(id);
        owner = htons(owner);
        flags = htons(flags);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GAnimationState {
    double x;                   // 8
    double y;                   // 8
    double accel_x;             // 8
    double accel_y;             // 8
    identifier_t id;            // 2
    scounter_t duration;        // 2
    identifier_t owner;         // 2

    inline void from_net() {
        id = ntohs(id);
        duration = ntohs(duration);
        owner = ntohs(owner);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }

    inline void to_net() {
        id = htons(id);
        duration = htons(duration);
        owner = htons(owner);
        byte_swap<double>(x);
        byte_swap<double>(y);
        byte_swap<double>(accel_x);
        byte_swap<double>(accel_y);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GPlayerRecoil {
    double x_recoil;                // 8
    identifier_t id;                // 2

    inline void from_net() {
        id = ntohs(id);
        byte_swap<double>(x_recoil);
    }

    inline void to_net() {
        id = htons(id);
        byte_swap<double>(x_recoil);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GTimeRemaining {
    pos_t remaining;                // 4
    flags_t flags;                  // 1

    inline void from_net() {
        remaining = ntohl(remaining);
    }

    inline void to_net() {
        remaining = htonl(remaining);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GTeamScore {
    GTeamScore() : score_red(0), score_blue(0) { }
    GTeamScore(score_t score_red, score_t score_blue)
        : score_red(score_red), score_blue(score_blue) { }

    score_t score_red;
    score_t score_blue;

    inline void from_net() {
        score_red = ntohs(score_red);
        score_blue = ntohs(score_blue);
    }

    inline void to_net() {
        score_red = htons(score_red);
        score_blue = htons(score_blue);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GFriendyFireAlarm {
    identifier_t owner;

    inline void from_net() {
        owner = ntohs(owner);
    }

    inline void to_net() {
        owner = htons(owner);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GTransportTime {
    sr_milliseconds_t ms;           // 4
    player_id_t id;                 // 2

    inline void from_net() {
        id = ntohs(id);
        ms = ntohl(ms);
    }

    inline void to_net() {
        id = htons(id);
        ms = htonl(ms);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GTransportTotalAndTime {
    sr_milliseconds_t total;        // 4
    sr_milliseconds_t current;      // 4
    player_id_t id;                 // 2

    inline void from_net() {
        id = ntohs(id);
        total = ntohl(total);
        current = ntohl(current);
    }

    inline void to_net() {
        id = htons(id);
        total = htonl(total);
        current = htonl(current);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GClanNames {
    char red_name[NameLength];
    char blue_name[NameLength];

    inline void from_net() { }
    inline void to_net() { }
};
#pragma pack()

// OK
#pragma pack(1)
struct GXferHeader {
    char filename[FilenameLen];
    datasize_t filesize;

    inline void from_net() {
        filesize = ntohl(filesize);
    }

    inline void to_net() {
        filesize = htonl(filesize);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GXferDataChunk {
    datasize_t chunksize;
    data_t data[1];

    inline void from_net() {
        chunksize = ntohl(chunksize);
    }

    inline void to_net() {
        chunksize = htonl(chunksize);
    }
};
#pragma pack()

// OK (rearranged)
#pragma pack(1)
struct GHillCounter {
    pos_t counter;                  // 4
    player_id_t id;                 // 2

    inline void from_net() {
        id = ntohs(id);
        counter = ntohl(counter);
    }

    inline void to_net() {
        id = htons(id);
        counter = htonl(counter);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GPakHash {
    static const int HashLength = 16;
    char pak_name[NameLength];
    char pak_hash[HashLength];  // CRC64

    inline void from_net() { }
    inline void to_net() { }
};
#pragma pack()

// OK
#pragma pack(1)
struct GI18NText {
    identifier_t id;                // 2
    data_len_t len;                 // 2
    data_t data[1];                 // 1

    inline void from_net() {
        id = ntohs(id);
        len = ntohs(len);
    }

    inline void to_net() {
        id = htons(id);
        len = htons(len);
    }
};
#pragma pack()

// OK
#pragma pack(1)
struct GChatMessage {
    player_id_t id;                 // 2
    data_len_t len;                 // 2
    data_t data[1];

    inline void from_net() {
        id = ntohs(id);
        len = ntohs(len);
    }

    inline void to_net() {
        id = htons(id);
        len = htons(len);
    }
};
#pragma pack()

const int GTransportLen = sizeof(GTransport) - 1;
const int GPlayerInfoLen = sizeof(GPlayerInfo);
const int GTournamentLen = sizeof(GTournament);
const int GTournamentSettingLen = sizeof(GTournamentSetting);
const int GPlayerStateLen = sizeof(GPlayerState);
const int GPTAllStatesLen = sizeof(GPTAllStates);
const int GPlayerClientStateLen = sizeof(GPlayerClientState);
const int GPlayerClientServerStateLen = sizeof(GPlayerClientServerState);
const int GPlayerServerStateLen = sizeof(GPlayerServerState);
const int GGameStateLen = sizeof(GGameState);
const int GPlayerDescriptionLen = sizeof(GPlayerDescription);
const int GPlaceObjectLen = sizeof(GPlaceObject);
const int GObjectStateLen = sizeof(GObjectState);
const int GAnimationStateLen = sizeof(GAnimationState);
const int GAnimationLen = sizeof(GAnimation);
const int GTextAnimationLen = sizeof(GTextAnimation);
const int GPlayerRecoilLen = sizeof(GPlayerRecoil);
const int GTimeRemainingLen = sizeof(GTimeRemaining);
const int GFriendyFireAlarmLen = sizeof(GFriendyFireAlarm);
const int GSpawnNPCLen = sizeof(GSpawnNPC);
const int GRemoveNPCLen = sizeof(GRemoveNPC);
const int GNPCStateLen = sizeof(GNPCState);
const int GSpawnObjectLen = sizeof(GSpawnObject);
const int GGenericNameLen = sizeof(GGenericName);
const int GTransportTimeLen = sizeof(GTransportTime);
const int GTransportTotalAndTimeLen = sizeof(GTransportTotalAndTime);
const int GClanNamesLen = sizeof(GClanNames);
const int GXferHeaderLen = sizeof(GXferHeader);
const int GXferDataChunkLen = sizeof(GXferDataChunk) - 1;
const int GHillCounterLen = sizeof(GHillCounter);
const int GPakHashLen = sizeof(GPakHash);
const int GI18NTextLen = sizeof(GI18NText) - 1;
const int GChatMessageLen = sizeof(GChatMessage) - 1;

#endif
