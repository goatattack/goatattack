#include "KeyValue.hpp"

#include <fstream>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cerrno>
#include <string>

KeyValue::KeyValue() : modified(false), depth_counter(0), hash_value(0), zip_file(false) { }

KeyValue::KeyValue(const std::string& filename, ZipReader *zip) throw (KeyValueException)
    : modified(false), depth_counter(0), hash_value(0), zip_file(zip ? true : false)
{
    if (filename.length()) {
        read(filename, zip);
        modified = false;
        if (zip) {
            zip_filename = zip->get_zip_filename();
            zip_file_hash = zip->get_hash();
        }
    }
}

KeyValue::~KeyValue() { }

const std::string& KeyValue::get_value(const std::string& key) const {
    Entries::const_iterator it = entries.find(key);
    if (it == entries.end()) {
        return empty_string;
    }

    return it->second;
}

void KeyValue::set_value(const std::string& key, const std::string& value) throw (KeyValueException) {
    modify_zip_file_test();
    if (!value.length()) {
        entries.erase(key);
    } else {
        entries[key] = value;
        modified = true;
    }
}

void KeyValue::set_value(const std::string& key, const char *value) throw (KeyValueException) {
    modify_zip_file_test();
    if (!strlen(value)) {
        entries.erase(key);
    } else {
        entries[key] = value;
        modified = true;
    }
}

void KeyValue::set_value(const std::string& key, int value) throw (KeyValueException) {
    modify_zip_file_test();
    char buffer[64];
    sprintf(buffer, "%d", value);
    entries[key] = buffer;
    modified = true;
}

void KeyValue::set_value(const std::string& key, double value) throw (KeyValueException) {
    modify_zip_file_test();
    char buffer[64];
    sprintf(buffer, "%f", value);
    entries[key] = buffer;
    modified = true;
}

void KeyValue::set_value(const std::string& key, bool value) throw (KeyValueException) {
    modify_zip_file_test();
    entries[key] = (value ? "1" : "0");
    modified = true;
}

void KeyValue::read(const std::string& filename, ZipReader *zip) throw (KeyValueException) {
    depth_counter++;
    if (depth_counter > 128) {
        throw KeyValueException("Too many nested includes: " + filename);
    }

    if (!zip) {
        /* regular file */
        std::ifstream f(filename.c_str());
        if (f.is_open()) {
            std::string line;
            while (getline(f, line)) {
                process_line(line, filename, zip);
            }
        } else {
            throw KeyValueException("Can't open key value file: " + filename);
        }
    } else {
        /* zip file */
        size_t sz;
        try {
            const char *data = zip->extract(filename, &sz);
            const char *dptr = data;
            std::string line;
            while (sz) {
                if (*dptr != '\r') {
                    if (*dptr == '\n') {
                        process_line(line, filename, zip);
                        line.clear();
                    } else {
                        line += *dptr;
                    }
                }
                dptr++;
                sz--;
            }
            zip->destroy(data);
        } catch (const ZipReaderException& e) {
            throw KeyValueException(e.what());
        }
    }
    depth_counter--;
}

void KeyValue::save(const std::string& filename) throw (KeyValueException) {
    modify_zip_file_test();
    std::ofstream f(filename.c_str());

    if (f.is_open()) {
        for (Entries::iterator it = entries.begin(); it != entries.end(); it++) {
            f << it->first << "=" << it->second << std::endl;
        }
    } else {
        throw KeyValueException("Write failed: " + std::string(strerror(errno)));
    }

    modified = false;
}

bool KeyValue::is_modified() const {
    return modified;
}

void KeyValue::touch() {
    modified = true;
}

void KeyValue::untouch() {
    modified = false;
}

const KeyValue::Entries& KeyValue::get_entries() const {
    return entries;
}

void KeyValue::clear() {
    entries.clear();
}

uint32_t KeyValue::get_hash_value() const {
    return hash_value;
}

bool KeyValue::is_zip_file() const {
    return zip_file;
}

const std::string& KeyValue::get_zip_filename() const {
    return zip_filename;
}

const std::string& KeyValue::get_zip_file_hash() const {
    return zip_file_hash;
}

void KeyValue::modify_zip_file_test() throw (KeyValueException) {
    if (is_zip_file()) {
        throw KeyValueException("A packaged file can't be modified.");
    }
}

void KeyValue::process_line(std::string line, const std::string& filename,
    ZipReader *zip) throw (KeyValueException)
{
    hash_value += get_hash_of_string(line.c_str());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    if (line.length()) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            if (key == "include") {
                size_t pos = filename.rfind('.');
                if (pos != std::string::npos) {
                    std::string suffix = filename.substr(pos);
                    pos = filename.rfind('/');
                    std::string prefix;
                    if (pos != std::string::npos) {
                        prefix = filename.substr(0, pos + 1);
                    }
                    std::string new_filename = prefix + value + suffix;
                    read(new_filename, zip);
                }
            } else {
                entries[key] = value;
            }
        } else {
            throw KeyValueException("Malformed expression in key value file: " + filename);
        }
    }
}
