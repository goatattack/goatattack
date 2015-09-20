#ifndef AUDIO_HPP
#define AUDIO_HPP

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
public:
    Audio();
    virtual ~Audio();

    virtual void generate_audio(const std::string& filename, ZipReader *zip = 0) throw (AudioException) = 0;
    virtual void generate_music(const std::string& filename, ZipReader *zip = 0) throw (AudioException) = 0;
};

#endif // AUDIO_HPP
