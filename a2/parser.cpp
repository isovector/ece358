#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

void get(size_t group_id, size_t student_id) {
  cout << "GETTING " << group_id << " and student " << student_id << endl;
}

void stop_session() {
  cout << "goodbye" << endl;
}

// Parses the client's commands and determines how to handle them
void parse(const char *buf)
{
    // Use a stringstream so we can avoid parsing the char* by hand
    stringstream str(buf);
    string command;

    str >> command;
    if (command == "GET")
    {
      int group_id, student_id;

      str >> group_id >> student_id;
      if (!str || group_id < 0 || student_id < 0)
      {
        // An error occurred. We're not sure what, but we handle them all
        // the same way.
        //
        // Since we have already read from the strstream, we can't get the
        // information out of it anymore, so shift our original buffer
        // pointer by the length of the prefix ("GET")
        //
        // It's kind of hacky, but at least it's well documented, right?
        cerr << "error: " << (buf + sizeof("GET")) << endl;
        return;
      }

      get(group_id, student_id);
    }
    else if (command == "STOP_SESSION")
    {
      stop_session();
    }
    else if (command == "STOP")
    {
      // There is nothing else to do, so gracefully shutdown the server.
      exit(0);
    }
    else
    {
      // An invalid command
      cerr << "error: invalid input" << endl;
    }
}

int main()
{
  char buffer[1024];
  while (cin.getline(buffer, 1024)) {
    parse(buffer);
  }
}

