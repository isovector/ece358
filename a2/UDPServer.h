#include "Server.h"

class UDPServer: public Server
{
	public:
		UDPServer();
		~UDPServer();

	private:
		virtual void get( int group_id, int student_id );
		virtual void stop_session();
		virtual void stop();
}
