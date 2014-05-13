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


  // YOLO SWAG 420 BLAZE IT
  const char *serialize() const {
    return reinterpret_cast<const char*>(this);
  }

  static const Message deserialize(const char *buffer) {
    return *reinterpret_cast<const Message*>(buffer);
  }
};

