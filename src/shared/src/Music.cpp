#include "Music.hpp"

Music::Music(Subsystem& subsystem, const std::string& filename, ZipReader *zip)
    throw (KeyValueException, MusicException)
    : Properties(filename + ".music", zip), subsystem(subsystem), audio(0)
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
}

Music::~Music() {
    delete audio;
}

const Audio *Music::get_audio() const {
    return audio;
}

ScopeMusicStopper::ScopeMusicStopper(Subsystem& subsystem, Music *music)
    : subsystem(subsystem), music(music)
{
    subsystem.stop_music();
}

ScopeMusicStopper::~ScopeMusicStopper() {
    if (music) {
        subsystem.play_music(music);
    }
}