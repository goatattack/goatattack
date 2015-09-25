#ifndef GAME_HPP
#define GAME_HPP

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "Resources.hpp"
#include "Configuration.hpp"

class GameException : public Exception {
public:
    GameException(const char *msg) : Exception(msg) { }
    GameException(std::string msg) : Exception(msg) { }
};

class Game {
private:
    Game(const Game&);
    Game& operator=(const Game&);

public:
    Game(Resources& resources, Subsystem& subsystem);
    virtual ~Game();

    void run(const std::string& parm);

private:
    Resources& resources;
    Subsystem& subsystem;
    Configuration config;
};

#endif
