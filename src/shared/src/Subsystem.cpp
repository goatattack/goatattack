#include "Subsystem.hpp"

Subsystem::Subsystem(std::ostream& stream, const std::string& window_title) throw (SubsystemException)
    : stream(stream), scanlines(0), keep_pictures(false) { }

Subsystem::~Subsystem() { }

void Subsystem::set_scanlines_icon(Icon *icon) {
    scanlines = icon;
}

void Subsystem::set_keep_pictures(bool state) {
    keep_pictures = state;
}

bool Subsystem::get_keep_pictures() const {
    return keep_pictures;
}

std::ostream& Subsystem::get_stream() const {
    return stream;
}