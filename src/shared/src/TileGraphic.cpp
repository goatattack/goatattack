#include "TileGraphic.hpp"

TileGraphic::TileGraphic(int width, int height, bool keep_pictures)
    : width(width), height(height), current_index(0),
      keep_pictures(keep_pictures) { }

TileGraphic::~TileGraphic() { }

int TileGraphic::get_width() {
    return width;
}

int TileGraphic::get_height() {
    return height;
}

int& TileGraphic::get_current_index() {
    return current_index;
}
