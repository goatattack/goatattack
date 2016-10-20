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

#include "KeyBinding.hpp"

static const char *Suffixes[] = { "", "_alternative" };

KeyBinding::KeyBinding() { }

KeyBinding::~KeyBinding() { }

void KeyBinding::extract_from_config(const Configuration& config) {
    for (int i = 0; i < MaxBindings; i++) {
        std::string suffix(Suffixes[i]);
        extract_binding(config, left[i], "left" + suffix);
        extract_binding(config, right[i], "right" + suffix);
        extract_binding(config, up[i], "up" + suffix);
        extract_binding(config, down[i], "down" + suffix);
        extract_binding(config, jump[i], "jump" + suffix);
        extract_binding(config, fire[i], "fire" + suffix);
        extract_binding(config, drop1[i], "drop1" + suffix);
        extract_binding(config, drop2[i], "drop2" + suffix);
        extract_binding(config, drop3[i], "drop3" + suffix);
        extract_binding(config, chat[i], "chat" + suffix);
        extract_binding(config, stats[i], "stats" + suffix);
        extract_binding(config, escape[i], "escape" + suffix);
    }
}

void KeyBinding::write_to_config(Configuration& config) {
    for (int i = 0; i < MaxBindings; i++) {
        std::string suffix(Suffixes[i]);
        write_binding(config, left[i], "left" + suffix);
        write_binding(config, right[i], "right" + suffix);
        write_binding(config, up[i], "up" + suffix);
        write_binding(config, down[i], "down" + suffix);
        write_binding(config, jump[i], "jump" + suffix);
        write_binding(config, fire[i], "fire" + suffix);
        write_binding(config, drop1[i], "drop1" + suffix);
        write_binding(config, drop2[i], "drop2" + suffix);
        write_binding(config, drop3[i], "drop3" + suffix);
        write_binding(config, chat[i], "chat" + suffix);
        write_binding(config, stats[i], "stats" + suffix);
        write_binding(config, escape[i], "escape" + suffix);
    }
}

void KeyBinding::extract_binding(const Configuration& config, MappedKey& binding, const std::string& key) {
    binding.device = static_cast<MappedKey::Device>(config.get_int("bind_" + key + "_device"));
    binding.param = config.get_int("bind_" + key + "_param");
}

void KeyBinding::write_binding(Configuration& config, MappedKey& binding, const std::string& key) {
    config.set_int("bind_" + key + "_device", static_cast<int>(binding.device));
    config.set_int("bind_" + key + "_param", binding.param);
}

const char *KeyBinding::get_suffix(int index) {
    return Suffixes[index];
}
