#pragma once

#include <stdint.h>
#include <cstddef>
#include <iostream>

// this is about the max size we can safely fit in a UDP packet
const size_t MAX_DATA_LENGTH = 500;

class msg_t
{
public:
    enum flags_t {
        NONE     = 0,
        ACK      = 1 << 0, // ack
        EOS      = 1 << 1, // end of send()
        FIN      = 1 << 2  // close channel
    };

    msg_t();
    msg_t(uint16_t seqnum, flags_t flags);
    msg_t(uint16_t seqnum, const char *srcData, size_t len, flags_t flags = NONE);

    uint32_t checksum;
    uint16_t seqnum;
    uint16_t length;
    flags_t flags;
    char data[MAX_DATA_LENGTH];

    bool valid() const;
    bool hasFlag(flags_t mask) const;
    size_t getTotalLength() const;

    const char *serialize() const;
    static const msg_t deserialize(const char *buffer);
};

msg_t::flags_t operator|(msg_t::flags_t a, msg_t::flags_t b);
std::ostream &operator<<(std::ostream &out, const msg_t &msg);

