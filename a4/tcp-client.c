/* Mahesh V. Tripunitara
 * University of Waterloo
 * tcp-client.c -- takes as cmd line args a server ip & port.
 * After establishing a connection, reads from stdin till eof
 * and sends everything it reads to the server. sleep()'s
 * occasionally in the midst of reading & sending.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

unsigned int getrand() {
    int f = open("/dev/urandom", O_RDONLY);
    if(f < 0) {
	perror("open(/dev/urandom)"); return 0;
    }

    unsigned int ret;
    read(f, &ret, sizeof(unsigned int));
    close(f);
    return ret;
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
	printf("usage: %s <server-ip> <server-port>\n", argv[0]);
	exit(0);
    }
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in a;

    memset(&a, 0, sizeof(struct sockaddr_in));
    a.sin_family = AF_INET;
    a.sin_port = 0;
    a.sin_addr.s_addr = INADDR_ANY;

    if(bind(s, (const struct sockaddr *)(&a), sizeof(struct sockaddr_in)) < 0) {
	perror("bind"); exit(1);
    }

    unsigned char buf[256];
    int nread = -1;

    a.sin_family = AF_INET;
    a.sin_port = htons((uint16_t)(atoi(argv[2])));
    if(inet_aton(argv[1], &(a.sin_addr)) < 0) {
    	fprintf(stderr, "inet_aton(%s) failed.\n", argv[1]);
    	exit(1);
    }

    if(connect(s, (const struct sockaddr *)(&a), sizeof(struct sockaddr_in)) < 0) {
    	perror("connect"); exit(1);
    }

    while((nread = read(STDIN_FILENO, buf, 256)) > 0) {

    	if(send(s, buf, nread, MSG_NOSIGNAL) < 0) {
    	    perror("send"); exit(1);
    	}

    	sleep(getrand()%7);
    }

    shutdown(s, SHUT_RDWR);
    close(s);

    return 0;
}
