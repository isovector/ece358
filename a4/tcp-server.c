/* Mahesh V. Tripunitara
 * University of Waterloo
 * tcp-server.c -- first prints out the IP & port on which in runs.
 * Then awaits connections from clients. Create a pthread for each
 * connection. The thread just reads what the other end sends and
 * write()'s it to a file whose name is the thread's ID.
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
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if 0
#define _DEBUG_
#endif

void *serviceConnection(void *arg) {
    int s = *(int *)arg;
    free(arg);

    char fname[256];
    memset(fname, 0, 256);
    snprintf(fname, 255, "%lu", pthread_self());

    int wfd = open(fname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if(wfd < 0) {
	perror("open"); return NULL;
    }

    unsigned char buf[256];
    ssize_t recvlen = 0;
    while((recvlen = recv(s, buf, 256, 0)) >= 0) {
#ifdef _DEBUG_
    	if(recvlen > 0) {
    	    printf("%lu received %d bytes.\n",
		    pthread_self(), recvlen);
    	}
#endif

	if(recvlen <= 0) {
	    /* quit */
#ifdef _DEBUG_
	    printf("%lu exiting, spot 1...\n", pthread_self());
#endif
	    close(wfd);
	    return NULL;
	}

#if 0
	if(write(STDOUT_FILENO, buf, recvlen) < recvlen) {
#endif
	if(write(wfd, buf, recvlen) < recvlen) {
	    perror("write() in thread wrote too few");
	    close(wfd);
	    return NULL;
	}
    }

#ifdef _DEBUG_
    printf("%lu exiting, spot 2...\n", pthread_self());
#endif
    close(wfd);
    return NULL;
}

uint32_t getPublicIPAddr() {
    struct ifaddrs *ifa;

    if(getifaddrs(&ifa) < 0) {
	perror("getifaddrs"); exit(0);
    }

    struct ifaddrs *c;
    for(c = ifa; c != NULL; c = c->ifa_next) {
	if(c->ifa_addr == NULL) continue;
	if(c->ifa_addr->sa_family == AF_INET) {
	    struct sockaddr_in a;

	    memcpy(&a, (c->ifa_addr), sizeof(struct sockaddr_in));
	    char *as = inet_ntoa(a.sin_addr);
	    //printf("%s\n", as);
	    
	    int apart;
	    sscanf(as, "%d", &apart);
	    if(apart > 0 && apart != 127) {
		freeifaddrs(ifa);
		return (a.sin_addr.s_addr);
	    }
	}
    }

    freeifaddrs(ifa);
    return 0;
}

int main(int argc, char *argv[]) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in a;

    memset(&a, 0, sizeof(struct sockaddr_in));
    a.sin_family = AF_INET;
    a.sin_port = 0;
    if((a.sin_addr.s_addr = getPublicIPAddr()) == 0) {
	fprintf(stderr, "Could not get public ip address. Exiting...\n");
	exit(0);
    }

    if(mybind(s, &a) < 0) {
	fprintf(stderr, "mybind() failed. Exiting...\n");
	exit(0);
    }

    printf("%s %u\n", inet_ntoa(a.sin_addr), ntohs(a.sin_port));
    
    if(listen(s, 0) < 0) {
	perror("listen"); exit(0);
    }

    memset(&a, 0, sizeof(struct sockaddr_in));
    socklen_t alen = sizeof(struct sockaddr_in);
    int asock;
    while((asock = accept(s, (struct sockaddr *)&a, &alen)) > 0) {
	int *newasock = (int *)malloc(sizeof(int));
	*newasock = asock;
	int err;
	pthread_t t;

	if(err = pthread_create(&t, NULL, &serviceConnection, (void *)(newasock))) {
	    fprintf(stderr, "pthread_create(): %s\n", strerror(err));
	    exit(1);
	}
    }

    return 0;
}
