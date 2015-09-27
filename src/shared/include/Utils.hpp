#ifndef UTILS_HPP
#define UTILS_HPP

#include "Exception.hpp"

#include <vector>
#include <string>
#ifdef __unix__
#include <arpa/inet.h>
extern const char *dir_separator;
#elif _WIN32
#include "Win.hpp"
#endif

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

class UtilsException : public Exception {
public:
    UtilsException(const char *msg) : Exception(msg) { }
    UtilsException(const std::string& msg) : Exception(msg) { }
};

typedef std::vector<std::string> StringTokens;

void create_directory(const std::string& directory, const std::string& in) throw (UtilsException);
bool is_directory(const std::string& path);
std::string get_home_directory() throw (UtilsException);
bool file_exists(const std::string& filename);
std::string ltrim(const std::string& s);
std::string rtrim(const std::string& s);
std::string trim(const std::string& s);
uint32_t resolve_host(const std::string& hostname);
int start_net();
void stop_net();
uint32_t get_hash_of_string(const char *str);
std::string uppercase(const std::string& str);
std::string lowercase(const std::string& str);
void instant_trim(std::string& str);
std::string generate_name();
StringTokens tokenize(const std::string& str, char delimiter, int count = 0);

#endif