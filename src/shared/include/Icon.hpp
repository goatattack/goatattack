#ifndef ICON_HPP
#define ICON_HPP

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Tile.hpp"
#include "ZipReader.hpp"

class IconException : public Exception {
public:
    IconException(const char *msg) : Exception(msg) { }
    IconException(const std::string& msg) : Exception(msg) { }
};

class Icon : public Properties {
private:
    Icon(const Icon&);
    Icon& operator=(const Icon&);

public:
    Icon(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, IconException);
    virtual ~Icon();

    Tile *get_tile();
    int get_hotspot_x() const;
    int get_hotspot_y() const;

private:
    Subsystem& subsystem;
    Tile *tile;
    int hotspot_x;
    int hotspot_y;

    void create_tile(const std::string& filename, ZipReader *zip) throw (Exception);
    void cleanup();
};

#endif
