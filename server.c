#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "server.h"
#include "register.h"

#ifdef DEBUG
#define dprintf(format, ...) fprintf(stderr, format __VA_OPT__(,) __VA_ARGS__)
#else
#define dprintf(format, ...)
#endif  // DEBUG

int main(int argc, char** argv) {
	int netfd;

	netfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (netfd == -1) {
		fprintf(stderr, "Failed to open socket\n");
		return -1;
	}

	// binding
	struct sockaddr_in addr = {.sin_family=AF_INET, .sin_port=htons(PORT_NUMBER), .sin_addr=htonl(INADDR_ANY)};
	if (bind(netfd, (struct sockaddr*) &addr, sizeof(addr))) {
		close(netfd);
		return -1;
	}

	// initialize database
	if (init()) {
		close(netfd);
		return -1;
	}

	uint8_t buf[UDP_BUFFER_SIZE];
	while (true) {
		// read udp message into buf
		size_t sz = recvfrom(netfd, buf, UDP_BUFFER_SIZE - 1, 0, (struct sockaddr*) &addr, NULL);
		if (sz == -1) {
			if (errno == EAGAIN || errno == EINTR) continue;
			fprintf(stderr, "Unknown reading error\n");
			return -1;
		}
		buf[sz] = '\0';

		// parse message
		if (strncmp(buf, KEYWORD_GET, KEYWORD_LEN)) {

		} else if (strncmp(buf, KEYWORD_REGISTER, KEYWORD_LEN) == 0) {
			
		} else if (strncmp(buf, KEYWORD_SELF_REGISTER, KEYWORD_LEN)) {

		} else if (strncmp(buf, KEYWORD_DEREGISTER, KEYWORD_LEN)) {

		} else {
			// ignore message
			dprintf(stderr, "Invalid message recieved: %s", buf);
			continue;
		}
	}

	close(netfd);
	cleanup();

	dprintf("Unreachable code reached\n");
	return -1;
}
