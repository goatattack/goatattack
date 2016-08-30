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

#ifndef _COMPILETHREADPIXEL_HPP_
#define _COMPILETHREADPIXEL_HPP_

#include "CompileThread.hpp"
#include "Thread.hpp"

class CompileThreadPixel : public CompileThread, private Thread {
public:
    CompileThreadPixel(EditableMap *wmap, unsigned char **lightmap);
    virtual ~CompileThreadPixel();

private:
    void thread();
};

#endif
