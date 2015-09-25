#include "Background.hpp"

#include <cstdlib>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

Background::Background(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, BackgroundException)
    : Properties(filename + ".background", zip), subsystem(subsystem), tilegraphics(0)
{
    char buffer[PATH_MAX];

    try {
        bool automatic = false;
        layers = atoi(get_value("layers").c_str());
        if (!layers) {
            automatic = true;
            layers = 1;
        }
        alpha = static_cast<float>(atof(get_value("alpha").c_str()));
        if (alpha < 0.1f) {
            alpha = 1.0f;
        }

        tilegraphics = new TileGraphic *[layers];
        for (int i = 0; i < layers; i++) {
            tilegraphics[i] = 0;
        }
        std::string pngfile;
        for (int i = 0; i < layers; i++) {
            if (automatic) {
                pngfile = filename + ".png";
            } else {
                sprintf(buffer, "%s%d.png", filename.c_str(), i);
                pngfile = buffer;
            }
            PNG png(pngfile, zip);
            tilegraphics[i] = subsystem.create_tilegraphic(png.get_width(), png.get_height());
            tilegraphics[i]->punch_out_tile(png, 0, 0, false);
        }
    } catch (const Exception& e) {
        cleanup();
        throw BackgroundException(e.what());
    }
}

Background::~Background() {
    cleanup();
}

TileGraphic *Background::get_tilegraphic(int index) {
    if (index < 0 || index >= layers) {
        index = 0;
    }
    return tilegraphics[index];
}

float Background::get_alpha() const {
    return alpha;
}

int Background::get_layer_count() const {
    return layers;
}

void Background::cleanup() {
    if (tilegraphics) {
        for (int i = 0; i < layers; i++) {
            if (tilegraphics[i]) {
                delete tilegraphics[i];
            }
        }
        delete[] tilegraphics;
    }
}
