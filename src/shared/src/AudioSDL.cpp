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

#ifndef DEDICATED_SERVER

#include "AudioSDL.hpp"

AudioSDL::AudioSDL() : chunk(0), music(0), data(0), src(0) { }

AudioSDL::~AudioSDL() {
    if (chunk) {
        Mix_FreeChunk(chunk);
    }

    if (music) {
        Mix_FreeMusic(music);
    }

    if (data) {
        ZipReader::destroy(data);
    }

    if (src) {
        SDL_RWclose(src);
    }
}

void AudioSDL::generate_audio(const std::string& filename, ZipReader *zip) throw (AudioException) {
    if (zip) {
        try {
            size_t sz;
            const char *data = zip->extract(filename, &sz);
            src = SDL_RWFromConstMem(const_cast<char *>(data), sz);
            chunk = Mix_LoadWAV_RW(src, 0);
            zip->destroy(data);
        } catch (const ZipReaderException& e) {
            throw AudioException(e.what());
        }
    } else {
        chunk = Mix_LoadWAV(filename.c_str());
    }
    if (!chunk) {
        throw AudioException("Could not open audio file " + filename + " (" +
            std::string(Mix_GetError()) + ")");
    }
}

void AudioSDL::generate_music(const std::string& filename, ZipReader *zip) throw (AudioException) {
    if (zip) {
        try {
            size_t sz;
            data = zip->extract(filename, &sz);
            src = SDL_RWFromConstMem(const_cast<char *>(data), sz);
            music = Mix_LoadMUS_RW(src, 0);
        } catch (const ZipReaderException& e) {
            throw AudioException(e.what());
        }
    } else {
        music = Mix_LoadMUS(filename.c_str());
    }
    if (!music) {
        throw AudioException("Could not open music file " + filename + " (" +
            std::string(Mix_GetError()) + ")");
    }
}

Mix_Chunk *AudioSDL::get_chunk() const {
    return chunk;
}

Mix_Music *AudioSDL::get_music() const {
    return music;
}

#endif
