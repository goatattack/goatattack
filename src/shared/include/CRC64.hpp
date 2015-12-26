#ifndef _CRC64_HPP_
#define _CRC64_HPP_

#include <stdint.h>
#include <string>

class CRC64 {
private:
    CRC64(const CRC64&);
    CRC64& operator=(const CRC64&);

public:
    CRC64();

    void process(const unsigned char *message, size_t len);
    uint64_t get_crc() const;
    std::string get_hash() const;

private:
    uint64_t crc;
};

#endif