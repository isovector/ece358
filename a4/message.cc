#include "message.h"
#include <assert.h>
#include <string.h>

msg_t::msg_t(const char *srcData, size_t len) :
    checksum(0xDEADBEEF),
    length(len)
{
    assert(len <= MAX_DATA_LENGTH);
    memcpy(data, srcData, len);
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

