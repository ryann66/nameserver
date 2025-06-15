#pragma once

#include <string.h>
#include <stdbool.h>

#include <netinet/ip.h>
#include <arpa/inet.h>

#include "server.h"

static inline void ip_to_string(struct sockaddr_in* ip, char buf[IPV4_MAX_LENGTH]) {
	strcpy(buf, inet_ntoa(ip->sin_addr));
}

static inline bool string_to_ip(struct sockaddr_in* ip, char buf[IPV4_MAX_LENGTH]) {
	return !inet_aton(buf, &ip->sin_addr);
}

static inline void port_to_string(struct sockaddr_in* ip, char buf[PORT_MAX_LENGTH]) {
	sprintf(buf, "%hu", &ip->sin_port);
}

static inline bool string_to_port(struct sockaddr_in* ip, char buf[PORT_MAX_LENGTH]) {
	return !sscanf(buf, "%hu", &ip->sin_port);
}
