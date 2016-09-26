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

#include "CompileThread.hpp"
#include "Scope.hpp"

#include <cmath>
#include <algorithm>

bool CompileThread::intersection(const Point& p1, const Point& p2,
    const Point& p3, const Point& p4, Point& intersection)
{
    float x1 = p1.x;
    float x2 = p2.x;
    float x3 = p3.x;
    float x4 = p4.x;

    float y1 = p1.y;
    float y2 = p2.y;
    float y3 = p3.y;
    float y4 = p4.y;

    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    /* if determinant is zero, there is no intersection, parallel lines */
    if (d == 0.0f) {
        return false;
    }

    /* get the x and y */
    float pre = (x1 * y2 - y1 * x2);
    float post = (x3 * y4 - y3 * x4);
    float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
    float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

    /* Check if the x and y coordinates are within both lines */
    if (x < std::min(x1, x2) || x > std::max(x1, x2) ||
        x < std::min(x3, x4) || x > std::max(x3, x4))
    {
        return false;
    }

    if (y < std::min(y1, y2) || y > std::max(y1, y2) ||
        y < std::min(y3, y4) || y > std::max(y3, y4))
    {
        return false;
    }

    /* return the point of intersection */
    intersection.x = x;
    intersection.y = y;
    return true;
}

CompileThread::CompileThread(EditableMap *wmap, unsigned char **lightmap)
    : wmap(wmap), lightmap(lightmap), finished(false), cancelled(false),
      finished_percent(0) { }

CompileThread::~CompileThread() { }

bool CompileThread::is_finished() {
    Scope<Mutex> lock(mtx);
    return finished;
}

bool CompileThread::is_cancelled() {
    Scope<Mutex> lock(mtx);
    return cancelled;
}

int CompileThread::get_percentage() {
    Scope<Mutex> lock(mtx);
    return finished_percent;
}

void CompileThread::cancel() {
    Scope<Mutex> lock(mtx);
    cancelled = true;
}
