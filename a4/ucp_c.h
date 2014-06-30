#pragma once

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>

int ucpSocket();
int ucpBind(int sockfd, const struct sockaddr_in *addr);
int ucpGetSockName(int sockfd, struct sockaddr_in *addr);
int ucpSetSockRecvTimeout(int sockfd, int milliSecs);
int ucpSendTo(int sockfd, const void *buf, int len, const struct sockaddr_in *to);
ssize_t ucpRecvFrom(int sockfd, void *buf, int len, struct sockaddr_in *from);
int ucpClose(int sockfd);

