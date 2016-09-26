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

#ifndef _COMPILETHREAD_HPP_
#define _COMPILETHREAD_HPP_

#include "EditableMap.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"

#include <string>

class CompileThread {
public:
    CompileThread(EditableMap *wmap, unsigned char **lightmap);
    virtual ~CompileThread() = 0;

    bool is_finished();
    bool is_cancelled();
    int get_percentage();
    void cancel();

protected:
    struct Point {
        Point() : x(0), y(0) { }
        Point(float x, float y) : x(x), y(y) { }
        float x;
        float y;
    };

    EditableMap *wmap;
    unsigned char **lightmap;
    bool finished;
    bool cancelled;
    int finished_percent;
    Mutex mtx;

    static bool intersection(const Point& p1, const Point& p2,
        const Point& p3, const Point& p4, Point& intersection);
};

#endif
