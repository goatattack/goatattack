#include "KeyBinding.hpp"

KeyBinding::KeyBinding() { }

KeyBinding::~KeyBinding() { }

void KeyBinding::extract_from_config(const Configuration& config) {
    extract_binding(config, left, "left");
    extract_binding(config, right, "right");
    extract_binding(config, up, "up");
    extract_binding(config, down, "down");
    extract_binding(config, jump, "jump");
    extract_binding(config, fire, "fire");
    extract_binding(config, drop1, "drop1");
    extract_binding(config, drop2, "drop2");
    extract_binding(config, drop3, "drop3");
    extract_binding(config, chat, "chat");
    extract_binding(config, stats, "stats");
    extract_binding(config, escape, "escape");
}

void KeyBinding::write_to_config(Configuration& config) {
    write_binding(config, left, "left");
    write_binding(config, right, "right");
    write_binding(config, up, "up");
    write_binding(config, down, "down");
    write_binding(config, jump, "jump");
    write_binding(config, fire, "fire");
    write_binding(config, drop1, "drop1");
    write_binding(config, drop2, "drop2");
    write_binding(config, drop3, "drop3");
    write_binding(config, chat, "chat");
    write_binding(config, stats, "stats");
    write_binding(config, escape, "escape");
}

void KeyBinding::extract_binding(const Configuration& config, MappedKey& binding, const std::string& key) {
    binding.device = static_cast<MappedKey::Device>(config.get_int("bind_" + key + "_device"));
    binding.param = config.get_int("bind_" + key + "_param");
}

void KeyBinding::write_binding(Configuration& config, MappedKey& binding, const std::string& key) {
    config.set_int("bind_" + key + "_device", static_cast<int>(binding.device));
    config.set_int("bind_" + key + "_param", binding.param);
}
