#ifndef UDPServer_H
#define UDPServer_H

#include <vector>

#include "Server.h"

class UDPServer: public Server
{
public:
    UDPServer();
    ~UDPServer();

    virtual void init();
    virtual void run();

protected:
    virtual bool handle_msg(int client, const char *msg);
    virtual void respond(int client, std::string reply);

    virtual void stop_session(int client);
    virtual void stop();

private:
    int sock_;
    sockaddr_in socket_address_;
    std::vector<sockaddr_in> clients_;

    int get_client_num(sockaddr_in source_addr);
    int remove_client(sockaddr_in source_addr);
};

#endif