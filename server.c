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
#include "utils.h"

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
	socklen_t addrlen;
	if (bind(netfd, (struct sockaddr*) &addr, sizeof(addr))) {
		close(netfd);
		fprintf(stderr, "Failed to bind socket\n");
		return -1;
	}

	uint8_t buf[UDP_BUFFER_SIZE];
	while (true) {
		addrlen = sizeof(struct sockaddr_in);

		// read udp message into buf
		size_t sz = recvfrom(netfd, buf, UDP_BUFFER_SIZE - 1, 0, (struct sockaddr*) &addr, &addrlen);
		if (sz == -1) {
			if (errno == EAGAIN || errno == EINTR) continue;
			fprintf(stderr, "Unknown reading error\n");
			return -1;
		}
		if (buf[sz-1] == '\n') sz--;
		buf[sz] = '\0';

		if (addr.sin_family != AF_INET) {
			// ignore message
			dprintf("Non-IPV4 packet recieved\n");
			continue;
		}

		// names must be at least 1 character long
		if (strlen(buf) < KEYWORD_LEN + 1) {
			// ignore message
			dprintf("Message too short to be a command\n");
			continue;
		}
		
		// parse message
		if (!strncmp(buf, KEYWORD_GET, KEYWORD_LEN)) {
			char* ebuf = buf + strlen(buf);
			struct sockaddr_in out;
			if (find_serv(buf+KEYWORD_LEN, &out)) {
				// change command (for response)
				strcpy(buf, KEYWORD_SUC_GET);

				// add delim after name
				ebuf[0] = DELIM[0];
				ebuf++;

				// copy in ip address
				ip_to_string(&out, ebuf);
				ebuf += strlen(ebuf);

				// add delim after ip addr
				ebuf[0] = DELIM[0];
				ebuf++;

				// copy in port number
				port_to_string(&out, ebuf);
				ebuf += strlen(ebuf);

				// null-terminate
				ebuf[0] = '\0';
			} else {
				strcpy(buf, KEYWORD_ERR_NOT_FOUND);
			}
		} else if (!strncmp(buf, KEYWORD_REGISTER, KEYWORD_LEN)) {
			struct sockaddr_in tmp;
			tmp.sin_family = AF_INET;

			// setup string components
			char* name = buf + KEYWORD_LEN;
			char* ip = strchr(name, DELIM[0]);
			if (ip == NULL) {
				dprintf("Invalid format\n");
				return 1;
			}
			ip[0] = '\0';
			ip++;
			char* port = strchr(ip, DELIM[0]);
			if (port == NULL) {
				dprintf("Invalid format\n");
				return 1;
			}
			port[0] = '\0';
			port++;

			if (string_to_ip(&tmp, ip)) {
				dprintf("Invalid register IP\n");
				continue;
			}
			if (string_to_port(&tmp, port)) {
				dprintf("Invalid register port\n");
				continue;
			}

			register_serv(name, tmp);
			
			strcpy(buf, KEYWORD_SUC_REG);
		} else if (!strncmp(buf, KEYWORD_SELF_REGISTER, KEYWORD_LEN)) {
			register_serv(buf + KEYWORD_LEN, addr);
			strcpy(buf, KEYWORD_SUC_REG);
		} else if (!strncmp(buf, KEYWORD_DEREGISTER, KEYWORD_LEN)) {
			if (deregister_serv(buf + KEYWORD_LEN)) {
				strcpy(buf, KEYWORD_SUC_DEREG);
			} else {
				strcpy(buf, KEYWORD_ERR_NOT_FOUND);
			}
		} else {
			// ignore message
			dprintf("Invalid message recieved: %s", buf);
			continue;
		}

		// send contents of buf back
		send:
		if (sendto(netfd, buf, strlen(buf) + 1, 0, (struct sockaddr*) &addr, sizeof(struct sockaddr_in)) == -1) {
			if (errno == EAGAIN || errno == EINTR) goto send;
			dprintf("Failed send, dropping\n");
			continue;
		}
	}

	close(netfd);
	cleanup();

	dprintf("Unreachable code reached\n");
	return -1;
}
