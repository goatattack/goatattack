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

#include "Sound.hpp"

Sound::Sound(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, SoundException)
    : Properties(filename + ".sound", zip), subsystem(subsystem), audio(0),
    playing_channel(-1)
{
    try {
        audio = subsystem.create_audio();
        audio->generate_audio(filename + ".ogg", zip);
    } catch (const AudioException& e) {
        if (audio) {
            delete audio;
            throw SoundException(e.what());
        }
    }
}

Sound::~Sound() {
    delete audio;
}

const Audio *Sound::get_audio() const {
    return audio;
}

int Sound::get_playing_channel() const {
    return playing_channel;
}

void Sound::set_playing_channel(int c) {
    playing_channel = c;
}
