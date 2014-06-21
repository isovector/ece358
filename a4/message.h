#pragma once

#include <stdint.h>
#include <cstddef>

const size_t MAX_DATA_LENGTH = 32;

class msg_t {
public: 
    msg_t(const char *srcData, size_t len);


    const uint32_t checksum;
    uint16_t seqnum;
    uint16_t length;
    enum {
        MSG_ACK = 1 << 0
    } flags;
    char data[MAX_DATA_LENGTH];

    size_t getTotalLength() const;
    const char *serialize() const;
    static const msg_t deserialize(const char *buffer);
};

