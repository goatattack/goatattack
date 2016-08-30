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

#ifndef _AUDIO_HPP_
#define _AUDIO_HPP_

#include "Exception.hpp"
#include "ZipReader.hpp"

#include <string>

#ifdef _WIN32
#include "Win.hpp"
#endif

class AudioException : public Exception {
public:
    AudioException(const char *msg) : Exception(msg) { }
    AudioException(const std::string& msg) : Exception(msg) { }
};

class Audio {
private:
    Audio(const Audio&);
    Audio& operator=(const Audio&);

public:
    Audio();
    virtual ~Audio();

    virtual void generate_audio(const std::string& filename, ZipReader *zip = 0) throw (AudioException) = 0;
    virtual void generate_music(const std::string& filename, ZipReader *zip = 0) throw (AudioException) = 0;
};

#endif
