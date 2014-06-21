#pragma once

#include "message.h"
#include <deque>

class msg_buffer_t {
public:
    msg_buffer_t();

    void queueMessage(const msg_t &msg);
    bool hasEnoughData(size_t length) const;
    void read(char *dest, size_t length);

private:
    std::deque<msg_t> buffer_;
    size_t totalSize_;
    size_t offset_;
};
