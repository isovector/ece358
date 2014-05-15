#ifndef TCPServer_H
#define TCPServer_H

#include "Server.h"
#include <vector>

class TCPServer : public Server {
public:
  TCPServer();
  virtual ~TCPServer() { }

  virtual void init();
  virtual void run();

protected:
  virtual void respond(int client, std::string reply);
  virtual bool handle_msg(int client, const char *msg);

  virtual void stop_session(int client);
  virtual void stop();

  std::vector<pollfd> clients_;
};

#endif