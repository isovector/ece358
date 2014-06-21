#include "harness.h"
#include <string>
#include <iostream>
#include "../msg_buffer.h"
using namespace std;

UNIT_TEST(MsgBufferTest) {
    msg_buffer_t buffer;
    EXPECT(!buffer.hasEnoughData(1));

    buffer.queueMessage(msg_t("abcdefghijklmnopqrstuvwxyz", 26));
    EXPECT(buffer.hasEnoughData(26));
    EXPECT(!buffer.hasEnoughData(27));

    char output[1024];
    buffer.read(output, 5);
    output[5] = 0;

    string tooLazyToDoStrCmp = output;
    EXPECT(tooLazyToDoStrCmp == "abcde");


    EXPECT(buffer.hasEnoughData(21));
    EXPECT(!buffer.hasEnoughData(22));

    buffer.queueMessage(msg_t("abcdefghijklmnopqrstuvwxyz", 26));
    EXPECT(buffer.hasEnoughData(47));

    buffer.read(output, 47);
    output[47] = 0;

    tooLazyToDoStrCmp = output;
    EXPECT(tooLazyToDoStrCmp == "fghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
}

