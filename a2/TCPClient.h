#ifndef TCPClient_H
#define TCPClient_H

#include "Client.h"

#include "Message.h"

class TCPClient : public Client
{
public:
    TCPClient();
    virtual ~TCPClient();

    virtual void set_host_info(std::string host_address, std::string host_port);

protected:
    virtual std::string send_message(Message msg);
    int host_port_;

private:
    int sock_;
    sockaddr_in addr_;
    sockaddr_in host_addr_;

};

#endif
