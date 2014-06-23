#include "harness.h"
#include <string>
#include <iostream>
#include "../msg_buffer.h"
using namespace std;

UNIT_TEST(MsgBufferTest) {
    msg_buffer_t buffer;

    EXPECT(buffer.empty());
    buffer.queueMessage(msg_t(0, "abcdefghijklmnopqrstuvwxyz", 26));
    buffer.queueMessage(msg_t(1, msg_t::EOS));
    buffer.queueMessage(msg_t(2, "abcdefghijklmnopqrstuvwxyz", 26));
    EXPECT(!buffer.empty());

    char output[1024];

    // test reading to max length
    output[buffer.read(output, 5)] = 0;
    string tooLazyToDoStrCmp = output;
    EXPECT(tooLazyToDoStrCmp == "abcde");
    cout << tooLazyToDoStrCmp << endl;

    // test reading to sequence point
    output[buffer.read(output, 47)] = 0;
    tooLazyToDoStrCmp = output;
    EXPECT(tooLazyToDoStrCmp == "fghijklmnopqrstuvwxyz");
    cout << tooLazyToDoStrCmp << endl;

    // test reading to end of buffer
    output[buffer.read(output, 47)] = 0;
    tooLazyToDoStrCmp = output;
    EXPECT(tooLazyToDoStrCmp == "abcdefghijklmnopqrstuvwxyz");
    cout << tooLazyToDoStrCmp << endl;

    EXPECT(buffer.empty());
}

