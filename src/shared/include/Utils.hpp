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

#ifndef _UTILS_HPP_
#define _UTILS_HPP_

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
void to_unicode(const char *src, wchar_t *dst, size_t out_len);
void from_unicode(const wchar_t *src, char *dst, size_t out_len);
void modify_directory_separator(std::string& s);

#endif
