#ifndef TOURNAMENTFACTORY_HPP
#define TOURNAMENTFACTORY_HPP

#include "Exception.hpp"
#include "Resources.hpp"
#include "Subsystem.hpp"
#include "Player.hpp"
#include "Gui.hpp"
#include "ServerLogger.hpp"
#include "Configuration.hpp"

#include <string>

class TournamentFactoryException : public Exception {
public:
    TournamentFactoryException(const char *msg) : Exception(msg) { }
    TournamentFactoryException(const std::string& msg) : Exception(msg) { }
};

struct MapConfiguration {
    MapConfiguration(GamePlayType type, const std::string& map_name,
        int duration, int warmup_in_seconds)
        : type(type), map_name(map_name), duration(duration),
          warmup_in_seconds(warmup_in_seconds) { }

    GamePlayType type;
    std::string map_name;
    int duration;
    int warmup_in_seconds;
};

class TournamentFactory {
public:
    TournamentFactory(Resources& resources, Subsystem& subsystem, Gui *gui)
        throw (TournamentFactoryException);
    virtual ~TournamentFactory();

    Tournament *create_tournament(const MapConfiguration& config,
        bool server, bool warmup, Players& players, ServerLogger *logger) throw (Exception);

    unsigned char get_tournament_id() const;
    void set_tournament_id(unsigned char id);

private:
    Resources& resources;
    Subsystem& subsystem;
    Gui *gui;
    unsigned char tournament_id;
};

#endif // TOURNAMENTFACTORY_HPP
