#include <string.h>
#include <stdbool.h>

#include <netinet/ip.h>

#include "register.h"
#include "server.h"

void register_serv(char serv[MAX_ENTRY_LENGTH], struct sockaddr_in ipp)
{
}

bool deregister_serv(char serv[MAX_ENTRY_LENGTH])
{
	return false;
}

bool find_serv(char serv[MAX_ENTRY_LENGTH], struct sockaddr_in *ipp_out)
{
	return false;
}
