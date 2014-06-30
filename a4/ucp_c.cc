/* 
 * Mahesh V. Tripunitara
 * University of Waterloo
 *
 */
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>

extern int errno;

unsigned int get_rand() {
    unsigned int ret = 0;
    int fd = open("/dev/urandom", O_RDONLY);

    if(fd < 0) {
	perror("catastrophic failure -- open(/dev/urandom)"); exit(1);
    }

    if(read(fd, &ret, sizeof(unsigned int)) < 0) {
	perror("catastrophic failure -- read(/dev/urandom)"); exit(1);
    }

    close(fd);
    return ret;
}

int ucpSocket()
{
    return(socket(AF_INET, SOCK_DGRAM, 0));
}

int ucpBind(int sockfd, const struct sockaddr_in *addr) {
    return(bind(sockfd, (const struct sockaddr *)addr,
		    (socklen_t)sizeof(struct sockaddr_in)));
}

int ucpGetSockName(int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrlen = (socklen_t)sizeof(struct sockaddr_in);
    return(getsockname(sockfd, (struct sockaddr *)addr, &addrlen));
}

int ucpSetSockRecvTimeout(int sockfd, int milliSecs)
{
    struct timeval t;

    t.tv_sec = milliSecs/1000; t.tv_usec = (milliSecs%1000)*1000;
    return(setsockopt(sockfd, (int)SOL_SOCKET, (int)SO_RCVTIMEO, (const void *)&t,
			(socklen_t)sizeof(t)));
}

int ucpSendTo(int sockfd, const void *buf, int len, const struct sockaddr_in *to)
{
    const int pDoEvil = 37; /* Chance in 100 that we will do evil */

    if(len <= 0) {
	errno = EINVAL; /* Invalid arg */
	return -1;
    }

    unsigned char sendBuf[len];
    bcopy(buf, (void *)sendBuf, (size_t)len);

    if(get_rand()%100 < pDoEvil) {
	/* What kind of evil? -- 1 of 3 kinds */

	int evilKind = get_rand()%3;
	    /* evilKind == 0 -- send only part of the bytes 
	     *             1 -- corrupt some of the bytes
	     *             2 -- don't send packet
	     */
	if(evilKind == 0) {
	    len = get_rand()%len; /* Guaranteed to be < len */
	}
	else if(evilKind == 1) {
	    int i;
	    for(i = 0; i < len; i++) {
		sendBuf[i] = (unsigned char)(get_rand()%256);
	    }
	}
	else {
	    /* Pretend we send all the bytes when we don't */
	    return len;
	}
    }

    return((int)sendto(sockfd, (const void *)sendBuf, (size_t)len, 0,
			(const struct sockaddr *)to,
    			(socklen_t)sizeof(struct sockaddr_in)));
}

ssize_t ucpRecvFrom(int sockfd, void *buf, int len, struct sockaddr_in *from)
{
    socklen_t addrlen = (socklen_t)sizeof(struct sockaddr_in);
    return((int)recvfrom(sockfd, buf, (size_t)len, 0, (struct sockaddr *)from,
			    &addrlen));
}

int ucpClose(int sockfd)
{
    return(close(sockfd));
}
