#ifdef TCP
#include "TCPClient.h"
TCPClient client;
#else
#include "UDPClient.h"
UDPClient client;
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;

// The client is provided the following two command-line arguments, in order.
//		(1) The IP address or domain name of the server.
//		 	Your client should be able to handle both.
//		(2) The port number on which the server runs.
int main(int argc, char *argv[])
{
    // Ensure command line arguments are provided
    if (argc != 3) {
        cerr << "error: invalid number of command line arguments" << endl;
        exit(0);
    } else {
        client.set_host_info(argv[1], argv[2]);
    }

    string line;

    while (getline(cin, line)) {
        int group_id,
            student_number;

        stringstream input;
        string command;
        char peekedChar;

        // It reads which group and student ID it should perform a GET
        //  for by reading from stdin.
        // input.flush();
        input.str(line);

        // Peek at input to determine command type
        peekedChar = input.peek();

        if (peekedChar == 'S') {
            // check if "STOP"
            input >> command;
            // cout << "command: " << command << endl;

            if (command == "STOP") {
                client.send_stop();
                exit(0);
            } else {
                cerr << "error: invalid input" << endl;
                continue;
            }
        } else if (peekedChar >= '0' && peekedChar <= '9') {
            // check validity of group/student num
            input >> group_id;
            // cout << "group_id: " << group_id << endl;

            // Grab rest of input
            input >> student_number;
            // cout << "student_number: " << student_number << endl;

            // Check validity of input
            if (group_id <= 0 || student_number <= 0) {
                cerr << "error: invalid input: " << group_id
                     << " " << student_number << endl;
                continue;
            } else {
                client.send_get(group_id, student_number);
            }

        } else {
            cerr << "error: invalid input" << endl;
        }

    }

    // If the client receives an EOF on stdin, it sends a
    //  STOP SESSION command to the server, and terminates itself.
    client.send_stop_session();


    // Sample client run
    // % ./client ecelinux3.uwaterloo.ca 5675
    // 1 1
    // Alice
    // 223 14
    // Bob Barker
    // 215 455
    // Charlie Horse
    // 23 11
    // error: 23 11
    // thisiscrazy
    // error: invalid input


}

