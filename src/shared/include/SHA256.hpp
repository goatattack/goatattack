#ifndef SHA256_HPP
#define SHA256_HPP

#include <string>

class SHA256 {
private:
    SHA256(const SHA256&);
    SHA256& operator=(const SHA256&);

public:
    typedef unsigned char uint8;
    typedef unsigned int uint32;

    SHA256();
    virtual ~SHA256();

    void process(const unsigned char *message, size_t len);
    void final();
    std::string get_hash();

private:
    static const unsigned int DigestSize = (256 / 8);
    static const unsigned int BlockSize = (512 / 8);
    const static uint32 sha256_constants[];
    unsigned char m_digest[DigestSize];
    unsigned int m_tot_len;
    unsigned int m_len;
    unsigned char m_block[2 * BlockSize];
    uint32 m_h[8];

    void init();
    void transform(const unsigned char *message, unsigned int block_nb);
};

#endif
