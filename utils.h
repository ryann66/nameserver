#pragma once

#include <string.h>

#include <netinet/ip.h>

#include "server.h"

void ip_to_string(struct sockaddr_in* ip, char buf[IPV4_MAX_LENGTH]);

void string_to_ip(struct sockaddr_in* ip, char buf[IPV4_MAX_LENGTH]);

void port_to_string(struct sockaddr_in* ip, char buf[PORT_MAX_LENGTH]);

void string_to_port(struct sockaddr_in* ip, char buf[PORT_MAX_LENGTH]);
