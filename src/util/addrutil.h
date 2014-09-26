#ifndef ADDRUTIL_H_
#define ADDRUTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

struct sockaddr_in createAddress(const char *ip, const int port);

int isEqualAddress(const struct sockaddr_in addrOne, const struct sockaddr_in addrTwo);

char *getIp(const struct sockaddr_in addr);

int getPort(const struct sockaddr_in addr);

#endif /* ADDRUTIL_H_ */