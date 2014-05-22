#ifndef UDPClient_H
#define UDPClient_H

#include "Client.h"

class UDPClient: public Client
{
public:
    UDPClient();
    virtual ~UDPClient();

    virtual void set_host_info(std::string host_address, std::string host_port);

protected:
    virtual std::string send_message(Message msg);

    int sock_;
    sockaddr_in addr_;
    sockaddr_in host_addr_;
};

#endif
