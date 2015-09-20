#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "Exception.hpp"
#include "KeyValue.hpp"

#include <string>

class ConfigurationException : public Exception {
public:
    ConfigurationException(const char *msg) : Exception(msg) { }
    ConfigurationException(const std::string& msg) : Exception(msg) { }
};

class Configuration {
public:
    Configuration(const std::string& directory, const std::string& filename) throw (ConfigurationException);
    virtual ~Configuration();

    std::string get_string(const std::string& key) const;
    void set_string(const std::string& key, const std::string& value);

    int get_int(const std::string& key) const;
    void set_int(const std::string& key, int value);

    bool get_bool(const std::string& key) const;
    void set_bool(const std::string& key, bool value);

    float get_float(const std::string& key) const;
    void set_float(const std::string& key, float value);

    bool get_show_player_name() const;
    const std::string& get_player_name() const;
    const std::string& get_player_skin() const;

    void extract();

private:
    std::string filename;
    KeyValue settings;
    char buffer[128];
    bool show_player_name;
    std::string player_name;
    std::string player_skin;

    void cdef(const std::string& key, const std::string& value);
};

#endif // CONFIGURATION_HPP
