#ifndef TILESET_HPP
#define TILESET_HPP

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "Properties.hpp"
#include "ZipReader.hpp"

#include <string>
#include <vector>

#ifdef _WIN32
#include "Win.hpp"
#endif

class TilesetException : public Exception {
public:
    TilesetException(const char *msg) : Exception(msg) { }
    TilesetException(const std::string& msg) : Exception(msg) { }
};

class Tileset : public Properties {
private:
    Tileset(const Tileset&);
    Tileset& operator=(const Tileset&);

public:
    Tileset(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, TilesetException);
    virtual ~Tileset();

    int get_tile_width();
    int get_tile_height();
    Tile *get_tile(int index);
    size_t get_tile_count();

private:
    typedef std::vector<Tile *> Tiles;

    Subsystem& subsystem;

    int tile_width;
    int tile_height;
    Tiles tiles;
    int sz;

    void create_tile(const std::string& filename, ZipReader *zip) throw (Exception);
    void cleanup();
};

#endif // TILESET_HPP
