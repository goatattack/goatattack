#include "MapConfiguration.hpp"

MapConfiguration::MapConfiguration()
    : type(GamePlayTypeDM), map_name(), duration(0), warmup_in_seconds(0) { }

MapConfiguration::MapConfiguration(GamePlayType type, const std::string& map_name,
    int duration, int warmup_in_seconds)
    : type(type), map_name(map_name), duration(duration),
      warmup_in_seconds(warmup_in_seconds) { }