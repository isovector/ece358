#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>
#include <vector>

#include "Message.h"

class Client
{
	public:
		Client();
		~Client();

		void send_get(int group_id, int student_id);
		void send_stop_session();
		void send_stop();

		virtual void set_host_info(std::string host_address, int host_port) = 0;

	protected:
		virtual std::string send_message(Message msg) = 0;

		std::string host_address;
		int host_port;

	private:
};
