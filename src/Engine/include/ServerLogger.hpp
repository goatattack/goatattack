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

#ifndef _SERVERLOGGER_HPP_
#define _SERVERLOGGER_HPP_

#include "Subsystem.hpp"
#include "Player.hpp"
#include "Map.hpp"

class ServerLogger {
private:
    ServerLogger(const ServerLogger&);
    ServerLogger& operator=(const ServerLogger&);

public:
    enum LogType {
        LogTypePlayerConnect = 0,   /* 000 */
        LogTypePlayerDisconnect,    /* 001 */
        LogTypeNewMap,              /* 002 */
        LogTypeChatMessage,         /* 003 */
        LogTypeWarmUp,              /* 004 */
        LogTypeGameBegins,          /* 005 */
        LogTypeGameOver,            /* 006 */
        LogTypeFrag,                /* 007 */
        LogTypeKill,                /* 008 */
        LogTypeRedTeamJoin,         /* 009 */
        LogTypeBlueTeamJoin,        /* 010 */
        LogTypeRedFlagPicked,       /* 011 */
        LogTypeRedFlagDropped,      /* 012 */
        LogTypeRedFlagSaved,        /* 013 */
        LogTypeRedFlagReturned,     /* 014 */
        LogTypeBlueFlagPicked,      /* 015 */
        LogTypeBlueFlagDropped,     /* 016 */
        LogTypeBlueFlagSaved,       /* 017 */
        LogTypeBlueFlagReturned,    /* 018 */
        LogTypeCoinPicked,          /* 019 */
        LogTypeCoinDropped,         /* 020 */
        LogTypeTeamRedScored,       /* 021 */
        LogTypeTeamBlueScored,      /* 022 */
        LogTypeRoundFinished,       /* 023 */
        LogTypeEndOfStats,          /* 024 */
        LogTypeStatsDM,             /* 025 */
        LogTypeStatsTDMTeamScore,   /* 026 */
        LogTypeStatsTDMTeamRed,     /* 027 */
        LogTypeStatsTDMTeamBlue,    /* 028 */
        LogTypeStatsCTFTeamScore,   /* 029 */
        LogTypeStatsCTFTeamRed,     /* 030 */
        LogTypeStatsCTFTeamBlue,    /* 031 */
        LogTypeStatsSR,             /* 032 */
        LogTypeStatsCTC,            /* 033 */
        LogTypeStatsGOHTeamScore,   /* 034 */
        LogTypeStatsGOHTeamRed,     /* 035 */
        LogTypeStatsGOHTeamBlue,    /* 036 */
        LogTypeJoin,                /* 037 */
        LogTypePlayerNameChange,    /* 038 */
        LogTypeMapClosed,           /* 039 */
        LogTypeLeft,                /* 040 */
        LogTypeInTheLobby,          /* 041 */
        _LogTypeMAX
    };

    ServerLogger(std::ostream& stream, bool verbose);
    virtual ~ServerLogger();

    void set_map(Map *map);
    void log(LogType type, const std::string& text, Player *p1 = 0, Player *p2 = 0,
        const void *data1 = 0, const void *data2 = 0,
        const void *data3 = 0, const void *data4 = 0);

private:
    std::ostream& stream;
    bool verbose;
    Map *map;

    char time_buffer[128];
    char buffer[4096];
    char subbuffer[256];
    std::string map_name;
    std::string map_description;

    void prepare();
    std::string make_quote(const std::string& s);
    const char *get_map_name();
    const char *get_map_description();
};

#endif
