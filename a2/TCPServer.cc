#include "TCPServer.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "Message.h"
using namespace std;

//  Description:
//
pollfd make_poll(int sock) {
  pollfd result;
  result.fd = sock;
  result.events = POLLIN;

  return result;
}

TCPServer::TCPServer()
    : Server()
{
}


//  Description:
//    The destructor closes all connections, including the listener socket
TCPServer::~TCPServer()
{
  for (vector<pollfd>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
    pollfd conn = *it;
    close(conn.fd);
  }

  clients_.clear();
}

//  Description:
//    Creates and binds the server's socket.
void TCPServer::init() {
  sockaddr_in socket_address;
  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock == -1) {
    cerr << "error: unable to create socket" << endl;
    exit(1);
  }

  // Set the socket to be non-blocking
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);

  // Determine our endpoint for the socket
  memset(&socket_address, 0, sizeof(sockaddr_in));
  socket_address.sin_family = AF_INET;

  int result;
  result = inet_pton(AF_INET, address_.c_str(), &socket_address.sin_addr);

  // Check result of conversion
  if (result < 0)
  {
    // Address conversion failed
    cerr << "error: first parameter is not a valid address family" << endl;
    close(sock);
    exit(1);
  }
  else if (result == 0)
  {
    // Address conversion failed
    cerr << "error: second parameter is not a valid IP address" << endl;
    close(sock);
    exit(1);
  }

  // Bind and listen to the socket
  result = get_port_and_bind(sock, &socket_address, sizeof(socket_address));
  if (result < 0) {
    cerr << "error: unable to bind to address " << endl;
    close(sock);
    exit(1);
  }

  // clients_ is a misnomer, since clients_[0] is not a client -- it is the
  // listener socket
  clients_.push_back(make_poll(sock));
}

//  Description:
//    What the server does until it receives a STOP command.
//    Check for incoming messages and send replies.
void TCPServer::run()
{
  int sock = clients_[0].fd;
  int result;

  result = listen(sock, 8);
  if (result == -1) {
    cerr << "error: unable to listen" << endl;
    exit(1);
  }

  // 3 minute timeout for polling
  size_t timeout = 3 * 60 * 1000;
  while (true) {
    result = poll(&clients_[0], clients_.size(), timeout);
    if (result < 0) {
      // poll() failed
      cerr << "error: socket broke" << endl;
      exit(1);
    }

    if (result == 0) {
      // poll() timed out
      cerr << "error: socket timeout" << endl;
      exit(1);
    }

    // poll() was successful; loop through the clients_ to see who
    // sent us a message.
    for (vector<pollfd>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
      pollfd &conn = *it;

      if (conn.revents == 0) {
        // Nothing happened on this connection
        continue;
      }

      if (conn.fd == sock) {
        // Someone wants to connect to us. Let's let them.
        while (true) {
          int new_client = accept(sock, NULL, NULL);
          if (new_client < 0) {
            if (errno == EWOULDBLOCK) {
              // This accept call would block -- we've accepted all incoming
              // connections.
              break;
            }

            // Otherwise we failed with a real error
            cerr << "error: accept returned a bad state" << endl;
            exit(errno);
          }

          // Set non-blocking and add this client to our list of
          // people we care about
          int flags = fcntl(new_client, F_GETFL, 0);
          fcntl(new_client, F_SETFL, flags | O_NONBLOCK);
          clients_.push_back(make_poll(new_client));
        }

        // Don't attempt to handle receiving logic, since this is the
        // listener socket.
        break;
      }

      // If we have made it to here, conn describes a client who has sent us
      // a message. Time to handle it!
      while (true) {
        char buffer[1024];
        result = recv(conn.fd, buffer, sizeof(buffer), 0);
        if (result < 0) {
          if (errno == EWOULDBLOCK) {
            // Done reading from this client.
            break;
          }

          // Fail otherwise
          cerr << "error: bad recv() from client (errno: " << errno << ")" << endl;
          exit(errno);
        }

        if (!handle_msg(conn.fd, buffer)) {
          // handle_msg() returns false if we should close the connection to
          // the requested client
          close(conn.fd);
          it = clients_.erase(it);

          // Yeah yeah I know it's ugly but we need to break out of TWO loops
          // and C++ doesn't do that
          goto endForLoop;
        }
      }
    } endForLoop:;
  }
}

//  Description:
//    Reads a message and acts according to the message type.
//  Input:
//    int client         : the client that sent the message
//    const char *buffer : the serialized message that was sent to the server
//  Returns:
//    true if the message was of type STOP_SESSION
bool TCPServer::handle_msg(int client, const char *buffer) {
  Message msg = Message::deserialize(buffer);
  switch (msg.command) {
    case Message::GET:
      respond(client, get(msg.param1, msg.param2));
      return true;

    case Message::STOP_SESSION:
      // We intentionally do not call stop_session() due to iterator woes -
      // this path actually executes in run()
      return false;

    case Message::STOP:
      stop();
      return true;

    default:
      cerr << "error: invalid command (corrupted message?)" << endl;
      return true;
  }
}

//  Description:
//    Sends a reply to a client.
//  Input:
//    int client      : the client to respond to (index for clients_)
//    string response : the server's response to the client's message
void TCPServer::respond(int client, string response) {
  send(client, response.c_str(), response.size(), 0);
}

//  Description:
//
void TCPServer::stop_session(int client) {
  // Intentionally left empty -- see corresponding code in run()
}

//  Description:
//    Close down the server.
void TCPServer::stop() {
  // The deconstructor will handle cleaning up the sockets for us
  exit(0);
}

