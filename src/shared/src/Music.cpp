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

#include "Music.hpp"

#include <cstdlib>

Music::Music(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, MusicException)
    : Properties(filename + ".music", zip), subsystem(subsystem), audio(0),
      do_not_play_in_music_player(false), audio_filename(filename)
{
    try {
        audio = subsystem.create_audio();
        audio->generate_music(filename + ".ogg", zip);
    } catch (const AudioException& e) {
        if (audio) {
            delete audio;
            throw MusicException(e.what());
        }
    }

    do_not_play_in_music_player = (atoi(get_value("do_not_play_in_music_player").c_str()) ? true : false);
}

Music::~Music() {
    if (audio) {
        delete audio;
    }
}

bool Music::get_do_not_play_in_music_player() const {
    return do_not_play_in_music_player;
}

const Audio *Music::get_audio() const throw (MusicException) {
    return audio;
}

ScopeMusicStopper::ScopeMusicStopper(Subsystem& subsystem, Music *music)
    : subsystem(subsystem), music(music), volume(subsystem.get_music_volume())
{
    subsystem.stop_music();
}

ScopeMusicStopper::~ScopeMusicStopper() {
    subsystem.set_music_volume(volume);
    if (music) {
        subsystem.play_music(music);
    }
}