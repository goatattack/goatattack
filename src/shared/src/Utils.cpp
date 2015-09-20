#include "Utils.hpp"

#include <cerrno>
#include <cstring>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cstdlib>
#ifdef __unix__
#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <sys/stat.h>
#include <netdb.h>
#include <sys/socket.h>
#include <SDL2/SDL.h>
#elif _WIN32
#include "Win.hpp"
#endif

#ifdef __unix__
const char *dir_separator = "/";
#endif

const char *name_syllables[] = {
    "ec", "mon", "aug", "iash", "fay", "mon", "shi", "tib", "ruk", "lar", "ka",
    "zag", "blarg", "rash", "izen", "malo", "zak", "hon", "fu", "tot", "be",
    "abo", "wonk", "lure", "ni", "mo", "kli", "gir", "stree", "lee", "mes",
    "dree", "weas", "rob", "faw", "raisk", "brud", "ouro", "auri", "reym", "ski",
    "bai", "on", "na", "nui", "han", "rol", "rays", "mon", "mian", "oro", "putt",
    "ro", "ran", "rin", "chin", "chan", "muli", "holo", "ron", "gan", "guyn",
    "ja", "red", "phi", "loin", "pho", "fran", "fre", "de", "ric", "sulu", "nau",
    0
};

void create_directory(const std::string& directory, const std::string& in) throw (UtilsException) {
#ifdef __unix__

    int rv = mkdir(dir.c_str(), S_IRWXU);
    if (rv && errno != EEXIST) {
        throw UtilsException(strerror(errno));
    }
#elif _WIN32
    CreateDirectoryA(dir.c_str(), 0);
#endif
}

bool is_directory(const std::string& path) {
#ifdef __unix__
    struct stat buffer;
    stat(path.c_str(), &buffer);
    return S_ISDIR(buffer.st_mode);
#elif _WIN32
    return (PathIsDirectoryA(path.c_str()) == (BOOL)FILE_ATTRIBUTE_DIRECTORY);
#endif
}

std::string get_home_directory() throw (UtilsException) {
#ifdef __unix__
  char * base_path = SDL_GetPrefPath("GoatAttack", "GoatAttack"); // ends with path sep
  if (base_path == NULL) {
    throw UtilsException("No base path available");
  }
  return std::string(base_path);
#elif _WIN32
    char path[MAX_PATH];
    if (!(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK)) {
        throw UtilsException("Function SHGetFolderPathA() failed.");
    }

    return std::string(path);
#endif
}

bool file_exists(const std::string& filename) {
#ifdef __unix__
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
#elif _WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE handle = FindFirstFileA(filename.c_str(), &ffd);
    bool found = (handle != INVALID_HANDLE_VALUE);
    if (found) {
        FindClose(handle);
    }
    return found;
#endif
}

std::string ltrim(const std::string& s) {
    std::string ns(s);

    ns.erase(ns.begin(), std::find_if(ns.begin(), ns.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return ns;
}

std::string rtrim(const std::string& s) {
    std::string ns(s);

    ns.erase(std::find_if(ns.rbegin(), ns.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), ns.end());
    return ns;
}

std::string trim(const std::string& s) {
    return ltrim(rtrim(s));
}

uint32_t resolve_host(const std::string& hostname) {
    if (hostname.length()) {
        struct addrinfo hints, *servinfo, *p;
        struct sockaddr_in *h;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if (getaddrinfo(hostname.c_str(), 0, &hints, &servinfo)) {
            return 0;
        }

        for (p = servinfo; p != NULL; p = p->ai_next)  {
            h = (struct sockaddr_in *)p->ai_addr;
    #ifdef __unix__
            return htonl(h->sin_addr.s_addr);
    #elif _WIN32
            return htonl(h->sin_addr.S_un.S_addr);
    #endif
        }

        freeaddrinfo(servinfo);
    }

    return 0;
}

int start_net() {
#ifdef _WIN32
    WSADATA wsa_data;
    return WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif
    return 1;
}

void stop_net() {
#ifdef _WIN32
    WSACleanup();
#endif
}

uint32_t get_hash_of_string(const char *str) {
    uint32_t h = 0;

    while (*str) {
       h = h << 1 ^ *str++;
    }

    return h;
}

std::string uppercase(const std::string& str) {
    std::string new_str(str);
    std::transform(new_str.begin(), new_str.end(), new_str.begin(), toupper);

    return new_str;
}

std::string generate_name() {
    const char **name = name_syllables;
    int syl_cnt = 0;
    while (*name) {
        name++;
        syl_cnt++;
    }

    int c = rand() % 3 + 2;
    std::string complete_name;

    for (int i = 0; i < c; i++) {
        name = name_syllables + (rand() % syl_cnt);
        complete_name += *name;
    }
    complete_name[0] = toupper(complete_name[0]);

    return complete_name;
}
