#ifndef KEYVALUE_HPP
#define KEYVALUE_HPP

#include "Exception.hpp"
#include "Utils.hpp"
#include "ZipReader.hpp"

#include <map>

#ifdef _WIN32
#include "Win.hpp"
#endif

class KeyValueException : public Exception {
public:
    KeyValueException(const char *msg) : Exception(msg) { }
    KeyValueException(const std::string& msg) : Exception(msg) { }
};

class KeyValue {
public:
    typedef std::map<std::string, std::string> Entries;

    KeyValue();
    KeyValue(const std::string& filename, ZipReader *zip = 0) throw (KeyValueException);
    virtual ~KeyValue();

    const std::string& get_value(const std::string& key) const;
    void set_value(const std::string& key, const std::string& value) throw (KeyValueException);
    void set_value(const std::string& key, const char *value) throw (KeyValueException);
    void set_value(const std::string& key, int value) throw (KeyValueException);
    void set_value(const std::string& key, double value) throw (KeyValueException);
    void set_value(const std::string& key, bool value) throw (KeyValueException);
    void read(const std::string& filename, ZipReader *zip = 0) throw (KeyValueException);
    void save(const std::string& filename) throw (KeyValueException);
    bool is_modified() const;
    void touch();
    void untouch();
    const Entries& get_entries() const;
    void clear();
    uint32_t get_hash_value() const;
    bool is_zip_file() const;
    const std::string& get_zip_filename() const;
    const std::string& get_zip_file_hash() const;

private:
    bool modified;
    int depth_counter;
    uint32_t hash_value;
    bool zip_file;

    Entries entries;
    std::string empty_string;
    std::string zip_filename;
    std::string zip_file_hash;

    void modify_zip_file_test() throw (KeyValueException);
    void process_line(std::string line, const std::string& filename, ZipReader *zip) throw (KeyValueException);
};

#endif
