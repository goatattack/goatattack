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

#ifndef _AUDIOSDL_HPP_
#define _AUDIOSDL_HPP_

#include "Audio.hpp"

#ifdef __unix__
#ifdef __APPLE__
#include <SDL2_mixer/SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#elif _WIN32
#include "Win.hpp"
#include <SDL_mixer.h>
#endif

class AudioSDL : public Audio {
public:
    AudioSDL();
    ~AudioSDL();

    virtual void generate_audio(const std::string& filename, ZipReader *zip = 0);
    virtual void generate_music(const std::string& filename, ZipReader *zip = 0);
    Mix_Chunk *get_chunk() const;
    Mix_Music *get_music() const;

private:
    Mix_Chunk *chunk;
    Mix_Music *music;
    const char *data;
    SDL_RWops *src;
};

#endif
