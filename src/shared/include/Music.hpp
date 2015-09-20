#ifndef MUSIC_HPP
#define MUSIC_HPP

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
public:
    Music(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, MusicException);
    virtual ~Music();

    const Audio *get_audio() const;

private:
    Subsystem& subsystem;
    Audio *audio;
};

#endif // MUSIC_HPP
