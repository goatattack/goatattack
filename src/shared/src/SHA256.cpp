#include "SHA256.hpp"

#include <cstdio>
#include <cstring>

static inline SHA256::uint32 sha2_ch(SHA256::uint32 x, SHA256::uint32 y, SHA256::uint32 z) {
    return ((x & y) ^ (~x & z));
}

static inline SHA256::uint32 sha2_maj(SHA256::uint32 x, SHA256::uint32 y, SHA256::uint32 z) {
    return ((x & y) ^ (x & z) ^ (y & z));
}

static inline SHA256::uint32 sha2_rotr(SHA256::uint32 x, int n) {
    return ((x >> n) | (x << ((sizeof(x) << 3) - n)));
}

static inline SHA256::uint32 sha2_shfr(SHA256::uint32 x, int n) {
    return (x >> n);
}

static inline SHA256::uint32 sha256_f1(SHA256::uint32 x) {
    return (sha2_rotr(x,  2) ^ sha2_rotr(x, 13) ^ sha2_rotr(x, 22));
}

static inline SHA256::uint32 sha256_f2(SHA256::uint32 x) {
    return (sha2_rotr(x,  6) ^ sha2_rotr(x, 11) ^ sha2_rotr(x, 25));
}

static inline SHA256::uint32 sha256_f3(SHA256::uint32 x) {
    return (sha2_rotr(x,  7) ^ sha2_rotr(x, 18) ^ sha2_shfr(x,  3));
}

static inline SHA256::uint32 sha256_f4(SHA256::uint32 x) {
    return (sha2_rotr(x, 17) ^ sha2_rotr(x, 19) ^ sha2_shfr(x, 10));
}

static inline void sha2_unpack32(SHA256::uint32 x, unsigned char *str) {
    *((str) + 3) = static_cast<SHA256::uint8>((x)      );
    *((str) + 2) = static_cast<SHA256::uint8>((x) >>  8);
    *((str) + 1) = static_cast<SHA256::uint8>((x) >> 16);
    *((str) + 0) = static_cast<SHA256::uint8>((x) >> 24);
}

static inline void sha2_pack32(const unsigned char *str, SHA256::uint32 *x) {
    *(x) =   (static_cast<SHA256::uint32>(*((str) + 3))      )
           | (static_cast<SHA256::uint32>(*((str) + 2)) <<  8)
           | (static_cast<SHA256::uint32>(*((str) + 1)) << 16)
           | (static_cast<SHA256::uint32>(*((str) + 0)) << 24);
}

const unsigned int SHA256::sha256_constants[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

SHA256::SHA256() {
    init();
}

SHA256::~SHA256() { }

void SHA256::process(const unsigned char *message, size_t len) {
    unsigned int block_nb;
    unsigned int new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;

    tmp_len = BlockSize - m_len;
    rem_len = len < tmp_len ? len : tmp_len;
    memcpy(&m_block[m_len], message, rem_len);
    if (m_len + len < BlockSize) {
        m_len += len;
        return;
    }
    new_len = len - rem_len;
    block_nb = new_len / BlockSize;
    shifted_message = message + rem_len;
    transform(m_block, 1);
    transform(shifted_message, block_nb);
    rem_len = new_len % BlockSize;
    memcpy(m_block, &shifted_message[block_nb << 6], rem_len);
    m_len = rem_len;
    m_tot_len += (block_nb + 1) << 6;
}

void SHA256::final() {
    unsigned int block_nb;
    unsigned int pm_len;
    unsigned int len_b;
    int i;

    block_nb = (1 + ((BlockSize - 9) < (m_len % BlockSize)));
    len_b = (m_tot_len + m_len) << 3;
    pm_len = block_nb << 6;
    memset(m_block + m_len, 0, pm_len - m_len);
    m_block[m_len] = 0x80;
    sha2_unpack32(len_b, m_block + pm_len - 4);
    transform(m_block, block_nb);
    for (i = 0 ; i < 8; i++) {
        sha2_unpack32(m_h[i], &m_digest[i << 2]);
    }
}

std::string SHA256::get_hash() {
    std::string hash;
    char buffer[16];

    for (unsigned int i = 0; i < DigestSize; i++) {
        sprintf(buffer, "%02x", m_digest[i]);
        hash += buffer;
    }

    return hash;
}

void SHA256::init() {
    m_h[0] = 0x6a09e667;
    m_h[1] = 0xbb67ae85;
    m_h[2] = 0x3c6ef372;
    m_h[3] = 0xa54ff53a;
    m_h[4] = 0x510e527f;
    m_h[5] = 0x9b05688c;
    m_h[6] = 0x1f83d9ab;
    m_h[7] = 0x5be0cd19;
    m_len = 0;
    m_tot_len = 0;
}

void SHA256::transform(const unsigned char *message, unsigned int block_nb) {
    uint32 w[64];
    uint32 wv[8];
    uint32 t1, t2;
    const unsigned char *sub_block;

    int block_sz = static_cast<int>(block_nb);
    for (int i = 0; i < block_sz; i++) {
        sub_block = message + (i << 6);
        for (int j = 0; j < 16; j++) {
            sha2_pack32(&sub_block[j << 2], &w[j]);
        }
        for (int j = 16; j < 64; j++) {
            w[j] =  sha256_f4(w[j -  2]) + w[j -  7] + sha256_f3(w[j - 15]) + w[j - 16];
        }
        for (int j = 0; j < 8; j++) {
            wv[j] = m_h[j];
        }
        for (int j = 0; j < 64; j++) {
            t1 = wv[7] + sha256_f2(wv[4]) + sha2_ch(wv[4], wv[5], wv[6]) + sha256_constants[j] + w[j];
            t2 = sha256_f1(wv[0]) + sha2_maj(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }
        for (int j = 0; j < 8; j++) {
            m_h[j] += wv[j];
        }
    }
}

