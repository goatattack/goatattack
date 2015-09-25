#ifndef KEYBINDING_HPP
#define KEYBINDING_HPP

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

    void extract_from_config(const Configuration& config);
    void write_to_config(Configuration& config);
    static void write_binding(Configuration& config, MappedKey& binding, const std::string& key);

    MappedKey left;
    MappedKey right;
    MappedKey up;
    MappedKey down;
    MappedKey jump;
    MappedKey fire;
    MappedKey drop1;
    MappedKey drop2;
    MappedKey drop3;
    MappedKey chat;
    MappedKey stats;
    MappedKey escape;

private:
    void extract_binding(const Configuration& config, MappedKey& binding, const std::string& key);
};

#endif
