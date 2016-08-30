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

#ifndef _SCOPEALLOCATOR_HPP_
#define _SCOPEALLOCATOR_HPP_

template <class T> class ScopeAllocator {
private:
    ScopeAllocator(const ScopeAllocator&);
    ScopeAllocator& operator=(const ScopeAllocator&);

public:
    ScopeAllocator() : obj(new T) { }
    virtual ~ScopeAllocator() { delete obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

template <class T> class ScopeArrayAllocator {
private:
    ScopeArrayAllocator(const ScopeArrayAllocator&);
    ScopeArrayAllocator& operator=(const ScopeArrayAllocator&);

public:
    ScopeArrayAllocator(int count) : obj(new T[count]) { }
    virtual ~ScopeArrayAllocator() { delete[] obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

#endif
