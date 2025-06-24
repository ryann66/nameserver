#pragma once

#include <stdbool.h>

#include <netinet/ip.h>

#include "server.h"

/**
 * Registers the service as under the given ip addr/port
 */
void register_serv(char serv[MAX_NAME_LENGTH], struct sockaddr_in ipp);

/**
 * Deregisters the given service
 * returns true if deregistered, false if not found
 */
bool deregister_serv(char serv[MAX_NAME_LENGTH]);

/**
 * Finds the service and stores the ip addr/port into the given sockaddr
 * returns true if found, false if not found
 */
bool find_serv(char serv[MAX_NAME_LENGTH], struct sockaddr_in* ipp_out);

/**
 * frees all entries in the database, freeing up RAM usage
 * 
 * Note: drops all registered server entries
 */
void cleanup();
