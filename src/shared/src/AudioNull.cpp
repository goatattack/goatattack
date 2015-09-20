#include "AudioNull.hpp"

AudioNull::AudioNull() { }

AudioNull::~AudioNull() { }

void AudioNull::generate_audio(const std::string& filename, ZipReader *zip) throw (AudioException) { }

void AudioNull::generate_music(const std::string& filename, ZipReader *zip) throw (AudioException) { }
