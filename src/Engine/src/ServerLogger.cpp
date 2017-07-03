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

#include "ServerLogger.hpp"

#include <cstdio>
#include <ctime>

ServerLogger::ServerLogger(std::ostream& stream, bool verbose)
    : stream(stream), verbose(verbose), map(0)
{
    prepare();
}

ServerLogger::~ServerLogger() { }

void ServerLogger::set_map(Map *map) {
    this->map = map;
    prepare();
}

void ServerLogger::log(LogType type, const std::string& text, Player *p1, Player *p2,
    const void *data1, const void *data2, const void *data3, const void *data4)
{
    if (verbose) {
        time_t t = time(0);
        strftime (time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", localtime(&t));

        switch (type) {
            case LogTypePlayerConnect:
            case LogTypePlayerDisconnect:
                /* TIME LOG TEXT PLAYER_NAME */
                sprintf(buffer, "%s %03d %s %s", time_buffer, type,
                    make_quote(text).c_str(),
                    make_quote(p1->get_player_name()).c_str());
                break;

            case LogTypeNewMap:
            case LogTypeMapClosed:
                /* TIME LOG MAP MAP_DESC TEXT */
                sprintf(buffer, "%s %03d %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str());
                break;

            case LogTypeChatMessage:
                /* TIME LOG MAP MAP_DESC TEXT PLAYER_NAME */
                sprintf(buffer, "%s %03d %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    make_quote(p1->get_player_name()).c_str());
                break;

            case LogTypeWarmUp:
            case LogTypeGameBegins:
            case LogTypeGameOver:
                /* TIME LOG MAP MAP_DESC TEXT */
                sprintf(buffer, "%s %03d %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str());
                break;

            case LogTypeFrag:
                /* TIME LOG MAP MAP_DESC TEXT FRAG_PLAYER KILL_PLAYER REASON */
                sprintf(buffer, "%s %03d %s %s %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    make_quote(p1->get_player_name()).c_str(),
                    make_quote(p2->get_player_name()).c_str(),
                    make_quote(static_cast<const char *>(data1)).c_str());
                break;

            case LogTypeKill:
                /* TIME LOG MAP MAP_DESC TEXT PLAYER_NAME REASON */
                sprintf(buffer, "%s %03d %s %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    make_quote(p1->get_player_name()).c_str(),
                    make_quote(static_cast<const char *>(data1)).c_str());
                break;

            case LogTypeRedTeamJoin:
            case LogTypeBlueTeamJoin:
            case LogTypeRedFlagPicked:
            case LogTypeRedFlagDropped:
            case LogTypeRedFlagSaved:
            case LogTypeBlueFlagPicked:
            case LogTypeBlueFlagDropped:
            case LogTypeBlueFlagSaved:
            case LogTypeCoinPicked:
            case LogTypeCoinDropped:
            case LogTypeJoin:
            case LogTypeLeft:
                /* TIME LOG MAP MAP_DESC TEXT PLAYER_NAME */
                sprintf(buffer, "%s %03d %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    make_quote(p1->get_player_name()).c_str());
                break;

            case LogTypeRedFlagReturned:
            case LogTypeBlueFlagReturned:
                /* TIME LOG MAP MAP_DESC TEXT */
                sprintf(buffer, "%s %03d %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str());
                break;

            case LogTypeTeamRedScored:
            case LogTypeTeamBlueScored:
                /* TIME LOG MAP MAP_DESC TEXT PLAYER_NAME */
                sprintf(buffer, "%s %03d %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    make_quote(p1->get_player_name()).c_str());
                break;

            case LogTypeRoundFinished:
                /* TIME LOG MAP MAP_DESC TEXT PLAYER TIME_IN_S */
                sprintf(buffer, "%s %03d %s %s %s %s %.2f", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    make_quote(p1->get_player_name()).c_str(),
                    *static_cast<const float *>(data1));
                break;

            case LogTypeEndOfStats:
                /* TIME LOG MAP MAP_DESC TEXT */
                sprintf(buffer, "%s %03d %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str());
                break;

            case LogTypeStatsTDMTeamScore:
            case LogTypeStatsCTFTeamScore:
            case LogTypeStatsGOHTeamScore:
                /* TIME LOG MAP MAP_DESC TEXT TEAM_RED SCORE TEAM_BLUE SCORE */
                sprintf(subbuffer, "%s %d %s %d",
                    make_quote(static_cast<const char *>(data1)).c_str(),
                    *static_cast<const score_t *>(data2),
                    make_quote(static_cast<const char *>(data3)).c_str(),
                    *static_cast<const score_t *>(data4));

                sprintf(buffer, "%s %03d %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    subbuffer);
                break;

            case LogTypeStatsDM:
            case LogTypeStatsTDMTeamRed:
            case LogTypeStatsTDMTeamBlue:
            case LogTypeStatsCTFTeamRed:
            case LogTypeStatsCTFTeamBlue:
            case LogTypeStatsGOHTeamRed:
            case LogTypeStatsGOHTeamBlue:
                /* TIME LOG MAP MAP_DESC TEXT RANK PLAYER SCORE FRAGS KILLS */
                sprintf(subbuffer, "%d %s %d %d %d", *static_cast<const int *>(data1),
                    make_quote(p1->get_player_name()).c_str(),
                    *static_cast<const sscore_t *>(data2),
                    *static_cast<const score_t *>(data3),
                    *static_cast<const score_t *>(data4));

                sprintf(buffer, "%s %03d %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    subbuffer);
                break;

            case LogTypeStatsSR:
                /* TIME LOG MAP MAP_DESC TEXT RANK PLAYER LAPS BEST LAST */
                if (!data3) {
                    sprintf(subbuffer, "%d %s %d N/A N/A", *static_cast<const int *>(data1),
                        make_quote(p1->get_player_name()).c_str(),
                        *static_cast<const int *>(data2));
                } else {
                    sprintf(subbuffer, "%d %s %d %.2f %.2f", *static_cast<const int *>(data1),
                        make_quote(p1->get_player_name()).c_str(),
                        *static_cast<const int *>(data2),
                        *static_cast<const float *>(data3),
                        *static_cast<const float *>(data4));
                }

                sprintf(buffer, "%s %03d %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    subbuffer);
                break;

            case LogTypeStatsCTC:
                /* TIME LOG MAP MAP_DESC TEXT RANK PLAYER MIN:SEC */
                sprintf(subbuffer, "%d %s %d:%02d", *static_cast<const int *>(data1),
                    make_quote(p1->get_player_name()).c_str(),
                    *static_cast<const int *>(data2),
                    *static_cast<const int *>(data3));

                sprintf(buffer, "%s %03d %s %s %s %s", time_buffer, type,
                    map_name.c_str(),
                    map_description.c_str(),
                    make_quote(text).c_str(),
                    subbuffer);
                break;

            case LogTypePlayerNameChange:
                /* TIME LOG TEXT OLD_NAME NEW_NAME */
                sprintf(buffer, "%s %03d %s %s %s", time_buffer, type,
                    make_quote(text).c_str(),
                    make_quote(static_cast<const char *>(data1)).c_str(),
                    make_quote(static_cast<const char *>(data2)).c_str());
                break;

            case _LogTypeMAX:
                /* do nothing */
                return;
                break;
        }
        stream << buffer << std::endl;
    } else {
        if (text.length()) {
            stream << text << std::endl;
        }
    }
}

void ServerLogger::prepare() {
    map_name = make_quote(get_map_name()).c_str();
    map_description = make_quote(get_map_description()).c_str();
}

std::string ServerLogger::make_quote(const std::string& s) {
    std::string quote = "\"";

    size_t sz = s.length();
    for (size_t i = 0; i < sz; i++) {
        quote += s[i];
        if (s[i] == '"') {
            quote += "\"";
        }
    }

    quote += "\"";
    return quote;
}

const char *ServerLogger::get_map_name() {
    if (map) {
        return map->get_name().c_str();
    } else {
        return "";
    }
}

const char *ServerLogger::get_map_description() {
    if (map) {
        return map->get_description().c_str();
    } else {
        return "";
    }
}
