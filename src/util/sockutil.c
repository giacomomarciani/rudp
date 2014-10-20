#include "sockutil.h"

/* SOCKET CREATION */

int openSocket() {
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ERREXIT("Cannot open socket.");

	return sock;
}

void closeSocket(const int sock) {

	if (close(sock) == -1)
		ERREXIT("Cannot close socket.");
}

void bindSocket(const int sock, const struct sockaddr_in *addr) {

	if (bind(sock, (const struct sockaddr *)addr, sizeof(struct sockaddr_in)) == -1) 
		ERREXIT("Cannot bind socket.");
}

/* SOCKET I/O */

void writeUnconnectedSocket(const int sock, const struct sockaddr_in rcvaddr, const char *buff) {
	socklen_t socksize = sizeof(struct sockaddr_in);

	if (sendto(sock, buff, strlen(buff), 0, (struct sockaddr *)&rcvaddr, socksize) == -1)
		ERREXIT("Cannot write to unconnected socket.");
}

char *readUnconnectedSocket(const int sock, struct sockaddr_in *sndaddr, const size_t rcvsize) {
	socklen_t socksize = sizeof(struct sockaddr_in);
	char *buff = NULL;
	ssize_t rcvd = 0;

	if (!(buff = malloc(sizeof(char) * (rcvsize + 1))))
		ERREXIT("Cannot allocate memory for reading unconnected socket.");

	if ((rcvd = recvfrom(sock, buff, rcvsize, 0, (struct sockaddr *)sndaddr, &socksize)) == -1) {

		if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
			free(buff);
			return NULL;
		}

		ERREXIT("Cannot read unconnected socket.");
	}

	buff[rcvd] = '\0';

	return buff;
}

void writeConnectedSocket(const int sock, const char *buff) {

	errno = 0;
	if (write(sock, buff, strlen(buff)) == -1) {

		if ((errno == EPIPE) | (errno == EINVAL))
			return;

		fprintf(stderr, "Cannot write connected socket: %s\n", strerror(errno));

		exit(EXIT_FAILURE);		
	}
}

char *readConnectedSocket(const int sock, const size_t rcvsize) {
	char *buff = NULL;
	ssize_t rcvd = 0;

	if (!(buff = malloc(sizeof(char) * (rcvsize + 1))))
		ERREXIT("Cannot allocate memory for reading connected socket.");

	errno = 0;

	if ((rcvd = read(sock, buff, rcvsize)) == -1)
		ERREXIT("Cannot read connected socket: %s\n", strerror(errno));

	buff[rcvd] = '\0';

	return buff;
}

/* SOCKET MULTIPLEXING */

int selectSocket(const int sock, long double millis) {
	fd_set readsock;
	struct timeval timer;
	int result;

	FD_ZERO(&readsock);

	FD_SET(sock, &readsock);

	timer = getTimeval(millis);

	errno = 0;

	if ((result = select(sock + 1, &readsock, NULL, NULL, &timer)) == -1)
		ERREXIT("Cannot select socket: %s.", strerror(errno));

	return result;
}

/* SOCKET PROPERTIES */

void setSocketConnected(const int sock, const struct sockaddr_in addr) {
	errno = 0;

	if (connect(sock, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1)
		ERREXIT("Cannot set socket connected: %s", strerror(errno));
}

void setSocketReusable(const int sock) {
	int optval = 1;

	errno = 0;

  	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int)) == -1)
		ERREXIT("Cannot set socket reusable: %s", strerror(errno));
}

void setSocketTimeout(const int sock, const uint8_t mode, const long double millis) {
	struct timeval timer;

	timer = getTimeval(millis);

	if (mode & ON_READ) {

		errno = 0;

  		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const void *)&timer, sizeof(timer)) < 0)
			ERREXIT("Cannot set socket read timeout: %s", strerror(errno));

	}

	if (mode & ON_WRITE) {

		errno = 0;

  		if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const void *)&timer, sizeof(timer)) < 0)
      		ERREXIT("Cannot set socket write timeout: %s", strerror(errno));

	}
}

/* SOCKET END-POINTS */

struct sockaddr_in getSocketLocal(const int sock) {
	socklen_t socksize = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;

	errno = 0;

	if (getsockname(sock, (struct sockaddr *)&addr, &socksize) == -1)
		ERREXIT("Cannot get socket local address: %s", strerror(errno));

	return addr;
}

struct sockaddr_in getSocketPeer(const int sock) {
	socklen_t socksize = sizeof(struct sockaddr_in);
	struct sockaddr_in addr;

	errno = 0;

	if (getpeername(sock, (struct sockaddr *)&addr, &socksize) == -1)
		ERREXIT("Cannot get socket peer address: %s", strerror(errno));

	return addr;
}
