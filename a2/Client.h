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

	protected:
		void send_get(int group_id, int student_id);
		void send_stop_session();
		void send_stop();

		virtual std::string send_message() = 0;


		void set_host_info(std::string host_address, int host_port);

		std::string host_address;
		int host_port;

	private:
};