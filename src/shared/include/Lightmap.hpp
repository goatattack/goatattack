#ifndef LIGHTMAP_HPP
#define LIGHTMAP_HPP

#include "Exception.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"

const int LightMapSize = 128;

class LightmapException : public Exception {
public:
    LightmapException(const char *msg) : Exception(msg) { }
    LightmapException(const std::string& msg) : Exception(msg) { }
};

class Lightmap {
public:
    Lightmap(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (LightmapException);
    virtual ~Lightmap();

    Tile *get_tile(int x, int y);
    void set_alpha(float alpha);
    float get_alpha() const;

private:
    Subsystem& subsystem;
    Tile ***tile;
    int width;
    int height;
    float alpha;

    void create_tile(const std::string& filename, ZipReader *zip) throw (Exception);
    void cleanup();
};


#endif // LIGHTMAP_HPP
