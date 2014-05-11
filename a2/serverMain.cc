#include <cstdlib>
#include <iostream>
#include <ctype.h>

#ifdef TCP
#include "TCPServer.h"
TCPServer server;
#else
#include "UDPServer.h"
UDPServer server;
#endif

int main(void)
{
	// The server ﬁrst reads stdin for group information
	// group IDs, each followed by a list of student IDs
	// and names that belong to that group,
	// with the input terminated by EOF
	int group_id;
	bool group_has_been_set = false;

	string command;
	while (cin >> command) {
	  if (command == "Group")
	  {
	    // Set the current Group ID
	    group_has_been_set = true;
	    cin >> group_id;
	  }
	  else if (isdigit(command[0]))
	  {
	    // Student lines begin with their student_id, but we need a group to put
	    // them into first.
	    if (!group_has_been_set)
	    {
	      cerr << "error: no group" << endl;
	      exit(1);
	    }

	    int student_id = atoi(command.c_str());

	    string name;
	    getline(cin, name);

	    // Due to how getline works, we'll get the deliminating space prefixed
	    // into the student's name, so remove it if it's present.
	    if (name[0] == ' ')
	    {
	      name.erase(0, 1);
	    }

	    server.add_student(group_id, student_id, name);
	  }
	  else
	  {
	    cerr << "error: bad input" << endl;
	    exit(2);
	  }
	}
}
