#include "msg_t.h"
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
    checksum(MSG_CHECKSUM),
    seqnum(seqnum),
    length(0),
    flags(flags)
{
}

msg_t::msg_t(uint16_t seqnum, const char *srcData, size_t len, flags_t flags) :
    checksum(MSG_CHECKSUM * (srcData[len - 1] + 1)),
    seqnum(seqnum),
    length(len),
    flags(flags)
{
    assert(len <= MAX_DATA_LENGTH);
    memcpy(data, srcData, len);
}

//  Description:
//    Determines if message has been corrupted by verifying
//    the checksum and length
//  Output:
//    bool : is the message valid?
bool msg_t::valid() const
{
    return length <= MAX_DATA_LENGTH
           && checksum == MSG_CHECKSUM  * (data[length - 1] + 1);
}

//  Description:
//    Determines if message has the given flag
//  Input:
//    flags_t mask : the flag that the message will be checked for
//  Output:
//    bool : does the message have the provided flag?
bool msg_t::hasFlag(flags_t mask) const
{
    return (flags & mask) == mask;
}

//  Description:
//    Calculates total length of the data payload contained in th message
//  Output:
//    size_t : the size of the payload
size_t msg_t::getTotalLength() const
{
    return sizeof(msg_t) - (MAX_DATA_LENGTH - length);
}

//  Description:
//    Serializes the message
//  Output:
//    const char * : the serialized message
const char *msg_t::serialize() const
{
    return reinterpret_cast<const char *>(this);
}

//  Description:
//    De-serializes the message
//  Output:
//    const char * : the serialized message
const msg_t msg_t::deserialize(const char *buffer)
{
    return *reinterpret_cast<const msg_t *>(buffer);
}

//  Description:
//    Allows us to OR message flags together
//  Output:
//    msg_t::flags_t : the flags OR'd together
msg_t::flags_t operator|(msg_t::flags_t a, msg_t::flags_t b)
{
    return msg_t::flags_t(int(a) | int(b));
}

//  Description:
//    Print message for packet inspection (DEBUGGING)
std::ostream &operator<<(std::ostream &out, const msg_t &msg)
{
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

