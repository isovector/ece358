#include "Server.h"

class TCPServer : public Server {
public:
  TCPServer() { }
  virtual ~TCPServer() { }
  
  virtual void run();
    
protected:
  virtual void respond(int client, std::string reply);
  virtual bool handle_msg(int client, const char *msg);

  virtual void stop_session();
  virtual void stop();
};

