#include "Properties.hpp"

Properties::Properties() { }

Properties::Properties(const std::string& filename, ZipReader *zip) throw (KeyValueException)
    : KeyValue(filename, zip), filename(filename)
{
    fetch();
}

Properties::~Properties() { }

const std::string& Properties::get_name() const {
    return name;
}

const std::string& Properties::get_author() const {
    return author;
}

const std::string& Properties::get_description() const {
    return description;
}

void Properties::set_name(const std::string& name) throw (KeyValueException) {
    set_value("name", name);
    this->name = name;
    touch();
}

void Properties::set_author(const std::string& author) throw (KeyValueException) {
    set_value("author", author);
    this->author = author;
    touch();
}

void Properties::set_description(const std::string& description) throw (KeyValueException) {
    set_value("description", description);
    this->description = description;
    touch();
}

void Properties::reload_configuration() throw (KeyValueException) {
    if (!filename.length()) {
        throw KeyValueException("No filename specified");
    }
    clear();
    read(filename);
    fetch();
}

void Properties::save_configuration() throw (KeyValueException) {
    if (!filename.length()) {
        throw KeyValueException("No filename specified");
    }
    save(filename);
}

void Properties::fetch() {
    name = get_value("name");
    author = get_value("author");
    description = get_value("description");
}