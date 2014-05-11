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

	private:
		void send_get( int group_id, int student_id );
		void send_stop_session();

		virtual std::string send_message() = 0;
};