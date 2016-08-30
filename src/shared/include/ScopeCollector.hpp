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

#ifndef _SCOPECOLLECTOR_HPP_
#define _SCOPECOLLECTOR_HPP_

template <class T> class ScopeCollector {
private:
    ScopeCollector(const ScopeCollector&);
    ScopeCollector& operator=(const ScopeCollector&);

public:
    ScopeCollector(T *obj) : obj(obj) { }
    virtual ~ScopeCollector() { delete obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

template <class T> class ScopeArrayCollector {
private:
    ScopeArrayCollector(const ScopeArrayCollector&);
    ScopeArrayCollector& operator=(const ScopeArrayCollector&);

public:
    ScopeArrayCollector(T *obj) : obj(obj) { }
    virtual ~ScopeArrayCollector() { delete[] obj; };

    T *operator*() { return obj; }

private:
    T *obj;
};

#endif
