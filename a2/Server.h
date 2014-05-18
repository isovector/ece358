#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <map>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <ifaddrs.h>

class Server
{
  public:
    Server();
    virtual ~Server();

    virtual void init() = 0;
    virtual void run() = 0;
    void add_student(int group_id, int student_id, std::string name);

    std::string get_client_connection_string() const;

  protected:
    // Message handling methods
    virtual bool handle_msg(int client, const char *msg) = 0;
    virtual void respond(int client, std::string reply) = 0;
    int get_port_and_bind(int sockfd, struct sockaddr_in *addr, size_t size);

    // Client handling methods
    std::string get(int group_id, int student_id);
    virtual void stop_session(int client) = 0;
    virtual void stop() = 0;

    std::map<int, std::map<int, std::string> > groups_;
    std::string address_;
    int port_;

  private:
    static bool is_good_interface(ifaddrs *ifap);
};

#endif
