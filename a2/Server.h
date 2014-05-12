#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

class Server
{
	public:
		Server() { }
		virtual ~Server() { }

    	virtual void run() = 0;
		void add_student(int group_id, int student_id, std::string name);

	protected:
		// Initialization
    	int get_port() const;

    	// Message handling methods
    	virtual bool handle_msg(int client, const char *msg) = 0;
    	virtual void respond(int client, std::string reply) = 0;

    	// Client handling methods
		void get(int group_id, int student_id);
		virtual void stop_session() = 0;
		virtual void stop() = 0;

    	std::map<int, std::map<int, std::string> > groups_;
};

