#ifndef AUDIOSDL_HPP
#define AUDIOSDL_HPP

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
    virtual ~AudioSDL();

    virtual void generate_audio(const std::string& filename, ZipReader *zip = 0) throw (AudioException);
    virtual void generate_music(const std::string& filename, ZipReader *zip = 0) throw (AudioException);
    Mix_Chunk *get_chunk() const;
    Mix_Music *get_music() const;

private:
    Mix_Chunk *chunk;
    Mix_Music *music;
    const char *data;
    SDL_RWops *src;
};

#endif // AUDIOSDL_HPP
