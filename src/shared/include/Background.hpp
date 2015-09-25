#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "TileGraphic.hpp"
#include "ZipReader.hpp"

class BackgroundException : public Exception {
public:
    BackgroundException(const char *msg) : Exception(msg) { }
    BackgroundException(const std::string& msg) : Exception(msg) { }
};

class Background : public Properties {
private:
    Background(const Background&);
    Background& operator=(const Background&);

public:
    Background(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
        throw (KeyValueException, BackgroundException);
    virtual ~Background();

    TileGraphic *get_tilegraphic(int index);
    float get_alpha() const;
    int get_layer_count() const;

private:
    Subsystem& subsystem;
    TileGraphic **tilegraphics;
    float alpha;
    int layers;

    void create_tile(const std::string& filename) throw (Exception);
    void cleanup();
};

#endif
