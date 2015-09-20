#ifndef AUDIONULL_HPP
#define AUDIONULL_HPP

#include "Audio.hpp"

#ifdef _WIN32
#include "Win.hpp"
#endif

class AudioNull : public Audio {
public:
    AudioNull();
    virtual ~AudioNull();

    virtual void generate_audio(const std::string& filename, ZipReader *zip = 0) throw (AudioException);
    virtual void generate_music(const std::string& filename, ZipReader *zip = 0) throw (AudioException);
};

#endif // AUDIONULL_HPP
