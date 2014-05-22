#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

struct Message {
    enum Command { GET = 0, STOP, STOP_SESSION };

    Command command;
    uint32_t param1;
    uint32_t param2;

    Message(Command command)
        : command(command), param1(0), param2(0) { }

    Message(Command command, uint32_t param1, uint32_t param2)
        : command(command), param1(param1), param2(param2) { }


    // WARNING! Here be dragons!
    // Reinterpretting an object as a byte stream for networking purposes
    // is in general a _really bad_ idea! If the underlying architectures
    // of the machines are different, this is GOING TO EXPLODE.
    //
    // Thankfully the ecelinux environment is homogenous, and our empirical
    // tests showed that this didn't explode. Still, we wouldn't recommend
    // doing this in a real environment.
    const char *serialize() const {
        return reinterpret_cast<const char *>(this);
    }

    static const Message deserialize(const char *buffer) {
        return *reinterpret_cast<const Message *>(buffer);
    }
};

#endif
