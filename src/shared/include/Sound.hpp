#ifndef SOUND_HPP
#define SOUND_HPP

#include "Exception.hpp"
#include "Properties.hpp"
#include "Subsystem.hpp"
#include "Audio.hpp"
#include "ZipReader.hpp"

class SoundException : public Exception {
public:
    SoundException(const char *msg) : Exception(msg) { }
    SoundException(const std::string& msg) : Exception(msg) { }
};

class Sound : public Properties {
private:
    Sound(const Sound&);
    Sound& operator=(const Sound&);

public:
    Sound(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, SoundException);
    virtual ~Sound();

    const Audio *get_audio() const;
    int get_playing_channel() const;
    void set_playing_channel(int c);

private:
    Subsystem& subsystem;
    Audio *audio;
    int playing_channel;
};

#endif
