#include "msg_buffer.h"
#include <algorithm>
#include <assert.h>
#include <string.h>
using namespace std;

msg_buffer_t::msg_buffer_t() :
    totalSize_(0),
    offset_(0)
{
}

void msg_buffer_t::queueMessage(const msg_t &msg) {
    buffer_.push_back(const_cast<msg_t&>(msg));
    totalSize_ += msg.length;
}

bool msg_buffer_t::hasEnoughData(size_t length) const {
    return totalSize_ >= length;
}

void msg_buffer_t::read(char *dest, size_t length) {
    assert(hasEnoughData(length));
    totalSize_ -= length;

    while (length > 0) {
        msg_t front = buffer_.front();
        size_t toCopy = min(length, front.length - offset_);

        memcpy(
            dest, 
            front.data + offset_,
            toCopy 
        );

        dest += toCopy;
        length -= toCopy;

        offset_ += toCopy;
        if (offset_ >= front.length) {
            offset_ = 0;
            buffer_.pop_front();
        }
    }
}

