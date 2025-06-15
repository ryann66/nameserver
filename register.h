#pragma once

#include <stdbool.h>

#include <netinet/ip.h>

#include "server.h"

/**
 * Initializes the database for normal operations
 */
int init();

/**
 * Registers the service as under the given ip addr/port
 */
void register_serv(char serv[MAX_ENTRY_LENGTH], struct sockaddr_in ipp);

/**
 * Deregisters the given service
 * returns true if deregistered, false if not found
 */
bool deregister_serv(char serv[MAX_ENTRY_LENGTH]);

/**
 * Finds the service and stores the ip addr/port into the given sockaddr
 * returns true if found, false if not found
 */
bool find_serv(char serv[MAX_ENTRY_LENGTH], struct sockaddr_in* ipp_out);

/**
 * Cleans up all resources used by database
 */
void cleanup();
