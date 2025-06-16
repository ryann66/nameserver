#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <netinet/ip.h>

#include "register.h"
#include "server.h"
#include "utils.h"

static FILE* dbfd;

// returns true if found, else false (and seeks to end of file)
static bool seek_to_serv(char serv[MAX_ENTRY_LENGTH]) {
	assert(fseek(dbfd, 0, SEEK_SET) == 0);
	char buf[FILE_LINE_LENGTH];
	while (fread(buf, sizeof(char), FILE_LINE_LENGTH, dbfd) > 0) {
		if (strncmp(buf, serv, MAX_ENTRY_LENGTH)) continue;
		assert(fseek(dbfd, -FILE_LINE_LENGTH, SEEK_CUR) == 0);
		return true;
	}
	return false;
}

int init() {
	dbfd = fopen(DB_FILEPATH, "ax");
	if (dbfd == NULL) return -1;
	return 0;
}

void register_serv(char serv[MAX_ENTRY_LENGTH], struct sockaddr_in ipp)
{
	char caclin[FILE_LINE_LENGTH];
	memset(caclin, 0, sizeof(caclin));
	strncpy(caclin, serv, MAX_ENTRY_LENGTH - 1);
	caclin[MAX_ENTRY_LENGTH] = DELIM[0];

	ip_to_string(&ipp, caclin + MAX_ENTRY_LENGTH + 1);
	caclin[MAX_ENTRY_LENGTH + IPV4_MAX_LENGTH + 1] = DELIM[0];

	port_to_string(&ipp, caclin + MAX_ENTRY_LENGTH + IPV4_MAX_LENGTH + 2);
	caclin[FILE_LINE_LENGTH - 1] = ENTRY_DELIM[0];

	seek_to_serv(serv);
	assert(fwrite(caclin, sizeof(char), FILE_LINE_LENGTH, dbfd) == FILE_LINE_LENGTH);
}

bool deregister_serv(char serv[MAX_ENTRY_LENGTH])
{
	char tmp[FILE_LINE_LENGTH];

	// if we can't seek, then the file must have no entries, thus failed deregister
	if (fseek(dbfd, -FILE_LINE_LENGTH, SEEK_END)) return false;
	long off = ftell(dbfd);

	assert(fread(tmp, sizeof(char), FILE_LINE_LENGTH, dbfd) == FILE_LINE_LENGTH);
	
	if (!seek_to_serv(serv)) {
		return false;
	}
	
	// overwrite dead spot
	assert(fwrite(tmp, sizeof(char), FILE_LINE_LENGTH, dbfd));
	
	// clear out end of file (sync to avoid libc/posix incompatibility)
	assert(fflush(dbfd) == 0);
	assert(ftruncate(fileno(dbfd), off) == 0);
	fsync(fileno(dbfd));

	return true;
}

bool find_serv(char serv[MAX_ENTRY_LENGTH], struct sockaddr_in *ipp_out)
{
	if (!seek_to_serv(serv)) return false;

	char buf[FILE_LINE_LENGTH];
	assert(fread(buf, sizeof(char), FILE_LINE_LENGTH, dbfd) == FILE_LINE_LENGTH);
	
	assert(!string_to_ip(ipp_out, buf + MAX_ENTRY_LENGTH + 1));
	assert(!string_to_port(ipp_out, buf + MAX_ENTRY_LENGTH + IPV4_MAX_LENGTH + 2));
	ipp_out->sin_family = AF_INET;

	return true;
}

void cleanup() {
	assert(fclose(dbfd));
}
