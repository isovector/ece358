#pragma once

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>

int rcsSocket();
int rcsBind(int sockfd, const struct sockaddr_in *addr);
int rcsGetSockName(int sockfd, struct sockaddr_in *addr);
int rcsListen(int sockfd);
int rcsAccept(int sockfd, struct sockaddr_in *addr);
int rcsConnect(int sockfd, const struct sockaddr_in *addr);
int rcsRecv(int sockfd, void *data, int size);
int rcsSend(int sockfd, const void *data, int size);
int rcsClose(int sockfd);

