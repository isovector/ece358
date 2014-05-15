#ifndef UDPServer_H
#define UDPServer_H

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

		virtual void stop_session( int client );
		virtual void stop();
};

#endif