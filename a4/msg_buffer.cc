#include "msg_buffer.h"
#include <algorithm>
#include <assert.h>
#include <string.h>
#include <iostream>
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

bool msg_buffer_t::empty() const {
    return totalSize_ == 0;
}

size_t msg_buffer_t::read(char *dest, size_t length) {
    assert(!empty());
    size_t written = 0;

    while (length > 0 && !buffer_.empty()) {
        msg_t front = buffer_.front();
        if (front.hasFlag(msg_t::EOS)) {
            buffer_.pop_front();
            if (written != 0) {
                break;
            } else {
                continue;
            }
        }

        size_t toCopy = min(length, front.length - offset_);

        memcpy(
            dest, 
            front.data + offset_,
            toCopy 
        );

        dest += toCopy;
        length -= toCopy;
        written += toCopy;

        offset_ += toCopy;
        if (offset_ >= front.length) {
            offset_ = 0;
            buffer_.pop_front();
        }
    }

    totalSize_ -= written;
    return written;
}

