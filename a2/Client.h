#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#include <string>
#include <map>
#include <vector>

#include "Message.h"

class Client
{
	public:
		Client();
		virtual ~Client();

		void send_get(int group_id, int student_id);
		void send_stop_session();
		void send_stop();

		virtual void set_host_info(std::string host_address, std::string host_port) = 0;

	protected:
		virtual std::string send_message(Message msg) = 0;
		int host_port_;
};

#endif
