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

#ifndef _BACKGROUND_HPP_
#define _BACKGROUND_HPP_

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
    Background(Subsystem& subsystem, const std::string& filename, ZipReader *zip);
    virtual ~Background();

    TileGraphic *get_tilegraphic(int index);
    float get_alpha() const;
    int get_layer_count() const;

private:
    Subsystem& subsystem;
    TileGraphic **tilegraphics;
    float alpha;
    int layers;

    void cleanup();
};

#endif
