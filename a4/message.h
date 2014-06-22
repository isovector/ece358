#pragma once

#include <stdint.h>
#include <cstddef>

const size_t MAX_DATA_LENGTH = 32;

class msg_t {
public: 
    enum flags_t {
        MSG_NONE = 0,
        MSG_ACK = 1 << 0,
        MSG_EOS = 1 << 1
    };

    msg_t();
    msg_t(uint16_t seqnum, flags_t flags);
    msg_t(uint16_t seqnum, const char *srcData, size_t len);

    uint32_t checksum;
    uint16_t seqnum;
    uint16_t length;
    flags_t flags;
    char data[MAX_DATA_LENGTH];

    bool valid() const;
    size_t getTotalLength() const;

    const char *serialize() const;
    static const msg_t deserialize(const char *buffer);
};

