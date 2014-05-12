#include "Server.h"

class UDPServer: public Server
{
	public:
		UDPServer();
		~UDPServer();

		virtual void run();
		void add_student(int group_id, int student_id, std::string name);

	protected:
		virtual bool handle_msg(int client, const char *msg);
		virtual void respond(int client, std::string reply);

		virtual void stop_session();
		virtual void stop();
};
