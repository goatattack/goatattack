/*
 *  This file is part of Goat Attack.
 *
 *  Goat Attack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Goat Attack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Goat Attack.  If not, see <http://www.gnu.org/licenses/>.
 */

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