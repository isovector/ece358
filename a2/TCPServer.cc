#include "TCPServer.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <errno.h>
#include "Message.h"
using namespace std;

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

void TCPServer::init() {
  sockaddr_in addr;
  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock == -1) {
    cerr << "error: unable to create socket" << endl;
    exit(1);
  }

  // Set the socket to be non-blocking
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);

  // Determine our endpoint for the socket
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // Bind and listen to the socket
  int result;
  result = get_port_and_bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
  if (result < 0) {
    cerr << "error: unable to bind to address " << endl;
    close(sock);
    exit(1);
  }

  result = listen(sock, 8);
  if (result == -1) {
    cerr << "error: unable to listen" << endl;
    close(sock);
    exit(1);
  }

  // clients_ is a misnomer, since clients_[0] is not a client -- it is the
  // listener socket
  clients_.push_back(make_poll(sock));
}

void TCPServer::run()
{
  int sock = clients_[0].fd;
  int result;

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
      pollfd conn = *it;

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

          // Add this client to our list of people we care about
          clients_.push_back(make_poll(new_client));
        }

        // Don't attempt to handle receiving logic, since this is the
        // listener socket.
        continue;
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
          cerr << "error: bad recv() from client" << endl;
          exit(errno);
        }

        if (!handle_msg(conn.fd, buffer)) {
          // handle_msg() returns false if we should close the connection to
          // the requested client
          close(conn.fd);
          it = clients_.erase(it);
        }
      }
    }
  }
}

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

void TCPServer::respond(int client, string response) {
  send(clients_[client].fd, response.c_str(), response.size(), 0);
}

void TCPServer::stop_session(int client) {
  // Intentionally left empty -- see corresponding code in run()
}

void TCPServer::stop() {
  // Close all connections, including our listener socket
  for (vector<pollfd>::iterator it = clients_.begin(); it != clients_.end(); ++it) {
    pollfd conn = *it;
    close(conn.fd);
  }

  clients_.clear();
  exit(0);
}

