#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <netinet/ip.h>

#include "register.h"
#include "server.h"
#include "utils.h"

#define streq(s1, s2) (strcmp((s1), (s2)) == 0)

struct cclin {
	struct sockaddr_in addr;
	char name[MAX_NAME_LENGTH];
};

#define TARGET_CHAIN_LEN 2

struct htb_node {
	struct cclin node;
	struct htb_node* next;
};

struct htb {
	// number of nodes in ht
	size_t util;

	// length of hashtable
	size_t len;
	
	struct htb_node** nodes;
};

struct htb serv_ht;

/**
 * doubles the hashtable size
 * 
 * returns 0 on success, else nonzero
 */
int ht_grow();

/**
 * adds a cache line to the RAM hashtable
 * does not take ownership of pointer
 * 
 * returns 0 on success, else nonzero
 */
int ht_insert(struct cclin* ent);

/**
 * returns the RAM address of a cache line in the RAM hashtable
 * does not relinquish ownership of pointer
 */
struct cclin* ht_find(char name[MAX_NAME_LENGTH]);

/**
 * Removes a cache line from the RAM hashtable
 * all references to ent will become invalid!
 * 
 * returns 0 on success, else nonzero on not found
 */
int ht_remove(char name[MAX_NAME_LENGTH]);


int ht_grow() {
	if (serv_ht.util / serv_ht.len <= TARGET_CHAIN_LEN) return 0;

	struct htb_node** narr = calloc(serv_ht.len * 2, sizeof(struct htb_node));
	if (narr == NULL) return -1;

	for (size_t i = 0; i < serv_ht.len; i++) {
		struct htb_node* nid = serv_ht.nodes[i];
		while (nid != NULL) {
			struct htb_node* tmp = nid->next;

			size_t hs = str_hash(nid->node.name);
			nid->next = narr[hs % (serv_ht.len * 2)];
			narr[hs % (serv_ht.len * 2)] = nid;

			nid = tmp;
		}
	}

	free(serv_ht.nodes);
	serv_ht.nodes = narr;
	serv_ht.len *= 2;

	return 0;
}

int ht_insert(struct cclin* ent) {
	struct htb_node* node = malloc(sizeof(struct htb_node));
	if (node == NULL) return 1;

	node->node = *ent;

	size_t hs = str_hash(ent->name);
	node->next = serv_ht.nodes[hs % serv_ht.len];
	serv_ht.nodes[hs % serv_ht.len] = node;

	serv_ht.util++;

	return ht_grow();
}

struct cclin* ht_find(char name[MAX_NAME_LENGTH]) {
	struct htb_node* nid = serv_ht.nodes[str_hash(name)];

	while (nid != NULL) {
		if (streq(nid->node.name, name)) {
			return &nid->node;
		}
		nid = nid->next;
	}

	// not found
	return NULL;
}

int ht_remove(char name[MAX_NAME_LENGTH]) {
	size_t hs = str_hash(name);
	struct htb_node* cn = serv_ht.nodes[hs % serv_ht.len];
	if (streq(cn->node.name, name)) {
		serv_ht.nodes[hs % serv_ht.len] = serv_ht.nodes[hs % serv_ht.len]->next;
		free(cn);
		return 0;
	} else {
		struct htb_node* ch = cn->next;
		while (ch != NULL) {
			if (streq(ch->node.name, name)) {
				cn->next = ch->next;
				free(ch);
				return 0;
			}

			cn = ch;
			ch = ch->next;
		}
	}

	// not found
	return 1;
}


void register_serv(char serv[MAX_NAME_LENGTH], struct sockaddr_in ipp) {

}

bool deregister_serv(char serv[MAX_NAME_LENGTH]) {

}

bool find_serv(char serv[MAX_NAME_LENGTH], struct sockaddr_in* ipp_out) {
	
}

void cleanup() {
	// cleanup hashtable entries
	for (size_t i = 0; i < serv_ht.len; i++) {
		struct htb_node* next = serv_ht.nodes[i];
		if (next == NULL) continue;

		while (next != NULL) {
			struct htb_node* tmp = next->next;
			free(next);
			next = tmp;
		}
	}

	// cleanup hashtable itself
	free(serv_ht.nodes);
	
	// reset fields to initialization values
	serv_ht.nodes = NULL;
	serv_ht.len = 0;
	serv_ht.util = 0;
}
