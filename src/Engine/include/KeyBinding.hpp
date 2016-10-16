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

#ifndef _KEYBINDING_HPP_
#define _KEYBINDING_HPP_

#include "Subsystem.hpp"
#include "Configuration.hpp"

struct MappedKey {
    enum Device {
        DeviceKeyboard = 0,
        DeviceJoyButton
    };

    MappedKey() : device(DeviceKeyboard), param(0) { }
    MappedKey(Device dev, int param) : device(dev), param(param) { }

    Device device;
    int param;
};

class KeyBinding {
public:
    KeyBinding();
    virtual ~KeyBinding();

    static const int MaxBindings = 2;

    void extract_from_config(const Configuration& config);
    void write_to_config(Configuration& config);
    static void write_binding(Configuration& config, MappedKey& binding, const std::string& key);
    static const char *get_suffix(int index);

    MappedKey left[MaxBindings];
    MappedKey right[MaxBindings];
    MappedKey up[MaxBindings];
    MappedKey down[MaxBindings];
    MappedKey jump[MaxBindings];
    MappedKey fire[MaxBindings];
    MappedKey drop1[MaxBindings];
    MappedKey drop2[MaxBindings];
    MappedKey drop3[MaxBindings];
    MappedKey chat[MaxBindings];
    MappedKey stats[MaxBindings];
    MappedKey escape[MaxBindings];

private:
    void extract_binding(const Configuration& config, MappedKey& binding, const std::string& key);
};

#endif
