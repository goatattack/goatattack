#include "Exception.hpp"

Exception::Exception(const char *caption) : caption(caption) { }

Exception::Exception(const std::string& caption) : caption(caption) { }

const char *Exception::what() const throw() {
    return this->caption.c_str();
}
