# ECE358 - A4

Alexander Maguire - 20396195
Ariel Weingarten  - 20366892


Implementation of rcs over top of ucp.

Files:

* rcs.cc:
    Wrapper around c-api for rcs* functions to our class-based socket solution.

* rcs_t.cc:
    C++ based rcs implementation -- most of the real code lives here.

* msg_t.cc:
    Message structure for socket protocol. Performs de/serialization.

* msg_buffer_t.cc
    ADT for reading and buffering of network messages. One exists per socket.

