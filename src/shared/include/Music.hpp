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

#ifndef _MUSIC_HPP_
#define _MUSIC_HPP_

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Audio.hpp"
#include "ZipReader.hpp"

class MusicException : public Exception {
public:
    MusicException(const char *msg) : Exception(msg) { }
    MusicException(const std::string& msg) : Exception(msg) { }
};

class Music : public Properties {
private:
    Music(const Music&);
    Music& operator=(const Music&);

public:
    Music(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0);
    ~Music();

    const Audio *get_audio() const;
    bool get_do_not_play_in_music_player() const;

private:
    Subsystem& subsystem;
    mutable Audio *audio;
    bool do_not_play_in_music_player;
    std::string audio_filename;
};

/* -------------------------------------------------------------------------- */

class ScopeMusicStopper {
private:
    ScopeMusicStopper(const ScopeMusicStopper&);
    ScopeMusicStopper& operator=(const ScopeMusicStopper&);

public:
    ScopeMusicStopper(Subsystem& subsystem, Music *music);
    virtual ~ScopeMusicStopper();

private:
    Subsystem& subsystem;
    Music *music;
};

#endif
