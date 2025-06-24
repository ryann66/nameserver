#pragma once

#include <assert.h>

/**
 * PROTOCOL:
 * UDP packets arrive on port 666 (likely broadcast)
 * Packets are human readable text, starting with a keyword (followed by DELIM)
 * 
 * Depending on the keyword, the following contents of the packet can be interpreted
 * KEYWORD_REGISTER : service_name : service_ip : service_port
 * KEYWORD_DEREGISTER : service_name
 * KEYWORD_GET : service_name
 * KEYWORD_ERR_NOT_FOUND : service_name
 * KEYWORD_SUC_DEREG : service_name
 * KEYWORD_SUC_REG : service_name
 * KEYWORD_SUC_GET : service_name : service_ip : service_port
 * 
 * NOTE: the actual protocol does not contain spaces, and it is forbidden for 
 *       service names to contain the DELIM character
 */

#define PORT_NUMBER 666

#define MAX_NAME_LENGTH 128

// length of ipv4 addresses
#define IPV4_MAX_LENGTH 15
// length of port numbers
#define PORT_MAX_LENGTH 5

#define DELIM ":"
static_assert(sizeof(DELIM) == 2);

// keywords for interacting with the database
#define KEYWORD_LEN 4
#define KEYWORD_REGISTER ("reg" DELIM)
#define KEYWORD_SELF_REGISTER ("srg" DELIM)
#define KEYWORD_DEREGISTER ("del" DELIM)
#define KEYWORD_GET ("get" DELIM)
#define KEYWORD_ERR_NOT_FOUND ("enf" DELIM)
#define KEYWORD_SUC_DEREG ("ded" DELIM)
#define KEYWORD_SUC_REG ("rok" DELIM)
#define KEYWORD_SUC_GET ("fnd" DELIM)

static_assert(sizeof(KEYWORD_REGISTER) == KEYWORD_LEN + 1);
static_assert(sizeof(KEYWORD_SELF_REGISTER) == KEYWORD_LEN + 1);
static_assert(sizeof(KEYWORD_DEREGISTER) == KEYWORD_LEN + 1);
static_assert(sizeof(KEYWORD_GET) == KEYWORD_LEN + 1);
static_assert(sizeof(KEYWORD_ERR_NOT_FOUND) == KEYWORD_LEN + 1);
static_assert(sizeof(KEYWORD_SUC_DEREG) == KEYWORD_LEN + 1);
static_assert(sizeof(KEYWORD_SUC_REG) == KEYWORD_LEN + 1);
static_assert(sizeof(KEYWORD_SUC_GET) == KEYWORD_LEN + 1);

// size of the database message buffer (must be able to hold all requests as well as largest response)
#define UDP_BUFFER_SIZE 512
static_assert(UDP_BUFFER_SIZE >= KEYWORD_LEN + 2 * (sizeof(DELIM) - 1) + IPV4_MAX_LENGTH + PORT_MAX_LENGTH);
