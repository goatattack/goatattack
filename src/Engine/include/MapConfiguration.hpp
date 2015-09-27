#ifndef MAPCONFIGURATION_HPP
#define MAPCONFIGURATION_HPP

#include "Resources.hpp"

#include <string>

struct MapConfiguration {
    MapConfiguration();
    MapConfiguration(GamePlayType type, const std::string& map_name,
        int duration, int warmup_in_seconds);

    GamePlayType type;
    std::string map_name;
    int duration;
    int warmup_in_seconds;
};

#endif