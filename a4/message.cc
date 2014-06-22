#include "message.h"
#include <assert.h>
#include <string.h>

const uint32_t MSG_CHECKSUM = 0xDEADBEEF;

msg_t::msg_t() :
    checksum(0),
    seqnum(0),
    length(0),
    flags(MSG_NONE)
{
}

msg_t::msg_t(uint16_t seqnum, flags_t flags) :
    checksum(MSG_CHECKSUM),
    seqnum(seqnum),
    length(0),
    flags(flags)
{
}

msg_t::msg_t(uint16_t seqnum, const char *srcData, size_t len) :
    checksum(MSG_CHECKSUM),
    seqnum(seqnum),
    length(len),
    flags(MSG_NONE)
{
    assert(len <= MAX_DATA_LENGTH);
    memcpy(data, srcData, len);
}

bool msg_t::valid() const {
    return checksum == MSG_CHECKSUM;
}

size_t msg_t::getTotalLength() const {
    return sizeof(msg_t) - (MAX_DATA_LENGTH - length);
}

const char *msg_t::serialize() const {
    return reinterpret_cast<const char*>(this);
}

const msg_t msg_t::deserialize(const char *buffer) {
    return *reinterpret_cast<const msg_t*>(buffer);
}

