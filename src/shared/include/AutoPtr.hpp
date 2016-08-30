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

#ifndef _AUTOPTR_HPP_
#define _AUTOPTR_HPP_

template<typename T> class AutoPtr {
private:
    AutoPtr(const AutoPtr&);
    const AutoPtr& operator=(const AutoPtr&);

public:
    AutoPtr(T *obj) : obj(obj)  { }
    ~AutoPtr()                  { reset(); }

    T *operator->()             { return obj; }
    T& operator*()              { return *obj; }
    bool valid()                { return obj != 0; }
    T *release()                { T *tmp = obj; obj = 0; return tmp; }
    void reset()                { delete obj; obj = 0; }

private:
    T *obj;
};

template<typename T> class AutoPtr<T[]> {
private:
    AutoPtr(const AutoPtr&);
    const AutoPtr& operator=(const AutoPtr&);

public:
    AutoPtr(T *obj) : obj(obj)  { }
    ~AutoPtr()                  { reset(); }

    T *operator->()             { return obj; }
    T& operator[](int index)    { return obj[index]; }
    bool valid()                { return obj != 0; }
    T *release()                { T *tmp = obj; obj = 0; return tmp; }
    void reset()                { delete[] obj; obj = 0; }

private:
    T *obj;
};

#endif
