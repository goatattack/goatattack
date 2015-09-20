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
