#include "message.h"
#include <assert.h>
#include <string.h>
using namespace std;

const uint32_t MSG_CHECKSUM = 16;

msg_t::msg_t() :
    checksum(0),
    seqnum(0),
    length(0),
    flags(NONE)
{
}

msg_t::msg_t(uint16_t seqnum, flags_t flags) :
    checksum(0),
    seqnum(seqnum),
    length(0),
    flags(flags)
{
}

msg_t::msg_t(uint16_t seqnum, const char *srcData, size_t len, flags_t flags) :
    checksum(MSG_CHECKSUM * len),
    seqnum(seqnum),
    length(len),
    flags(flags)
{
    assert(len <= MAX_DATA_LENGTH);
    memcpy(data, srcData, len);
}

bool msg_t::valid() const {
    return checksum == MSG_CHECKSUM  * length
        && length <= MAX_DATA_LENGTH;
}

bool msg_t::hasFlag(flags_t mask) const {
    return (flags & mask) == mask;
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

msg_t::flags_t operator|(msg_t::flags_t a, msg_t::flags_t b) {
    return msg_t::flags_t(int(a) | int(b));
}

std::ostream &operator<<(std::ostream &out, const msg_t &msg) {
    if (!msg.valid()) {
        out << "msg_t[BAD]";
        return out;
    }

    char buffer[MAX_DATA_LENGTH];
    memcpy(buffer, msg.data, msg.length);
    buffer[msg.length] = 0; 

    out << "msg_t[" << msg.seqnum << "] "
        << (msg.flags == msg_t::ACK ? "ack/" :
            msg.flags == msg_t::EOS ? "eos/" :
            msg.flags == msg_t::FIN ? "fin/" : "")
        << msg.length << ": " <<  buffer;

    return out;
}

