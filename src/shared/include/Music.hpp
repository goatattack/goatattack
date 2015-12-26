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
private:
    Music(const Music&);
    Music& operator=(const Music&);

public:
    Music(Subsystem& subsystem, const std::string& filename, ZipReader *zip = 0)
        throw (KeyValueException, MusicException);
    virtual ~Music();

    const Audio *get_audio() const throw (MusicException);
    bool get_do_not_play_in_music_player() const;

private:
    Subsystem& subsystem;
    mutable Audio *audio;
    bool do_not_play_in_music_player;
    std::string audio_filename;
};

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
    int volume;
};

#endif
