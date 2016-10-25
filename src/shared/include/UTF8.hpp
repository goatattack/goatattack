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

#ifndef _UTF8_HPP_
#define _UTF8_HPP_

#include <stdint.h>
#include <cstdlib>

/*
 * Parts of this code is based on the utf-8 decoder of Bjoern Hoehrmann.
 * See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
 */

static const int UTF8_ACCEPT = 0;
static const int UTF8_REJECT = 12;

static const uint8_t _utf8_masks[] = {
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8
};

static const int _utf8_states[] = {
    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
     0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12
};

inline int utf8_decode(const unsigned char byte, int& state, uint32_t& codep) {
    uint32_t type = _utf8_masks[byte];
    codep = (state != UTF8_ACCEPT) ? (byte & 0x3fu) | (codep << 6) : (0xff >> type) & (byte);
    state = _utf8_states[state + type];

    return state;
}

inline size_t utf8_strlen(const char *s) {
    int state = UTF8_ACCEPT;
    int len = 0;
    for (uint32_t codepoint; *s; ++s) {
        if (!utf8_decode(*s, state, codepoint)) {
            len++;
        }
    }

    return len;
}

template<typename T> inline bool fast_mbstowcs(T *out, const char *s, size_t len) {
    int state = UTF8_ACCEPT;

    for (uint32_t codepoint; len; ++s) {
        if (!utf8_decode(*reinterpret_cast<const unsigned char *>(s), state, codepoint)) {
            *out++ = codepoint;
            len--;
        }
        if (!*s) {
            break;
        }
    }

    return (state == UTF8_ACCEPT);
}

template<typename T> inline bool fast_mbtowc(T& out, const char *s) {
    return fast_mbstowcs(&out, s, 1);
}

#endif