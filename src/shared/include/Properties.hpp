#ifndef PROPERTIES_HPP
#define PROPERTIES_HPP

#include "KeyValue.hpp"
#include "ZipReader.hpp"

#ifdef _WIN32
#include "Win.hpp"
#endif

class Properties : public KeyValue {
public:
    Properties();
    Properties(const std::string& filename, ZipReader *zip = 0) throw (KeyValueException);
    virtual ~Properties();

    const std::string& get_name() const;
    const std::string& get_author() const;
    const std::string& get_description() const;

    void set_name(const std::string& name) throw (KeyValueException);
    void set_author(const std::string& author) throw (KeyValueException);
    void set_description(const std::string& description) throw (KeyValueException);

private:
    std::string name;
    std::string author;
    std::string description;
};

#endif // PROPERTIES_HPP
