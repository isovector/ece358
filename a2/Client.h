#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>
#include <vector>

class Client
{
	public:
		Client();
		~Client();

		void send_get(int group_id, int student_id);
		void send_stop_session();
		void send_stop();

		void set_host_info(std::string host_address, int host_port);

	protected:
		virtual std::string send_message() = 0;

		std::string host_address;
		int host_port;

	private:
};