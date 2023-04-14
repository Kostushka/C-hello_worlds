#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsinghttp.h"

unsigned hashfunc(struct Hash *hash, char *key) {
	unsigned hashvalue;
	for (hashvalue = 0; *key != '\0'; key++) {
		hashvalue = *key + 31 * hashvalue;
	}
	return hashvalue % hash->size_hashtab;
}

struct Block *find_hash(struct Hash *hash, char *key) {
	struct Block *p;
	for (p = hash->hashtab[hashfunc(hash, key)]; p != NULL; p = p->p) {
		if (strcmp(p->key, key) == 0) {
			return p;
		}
	}
	return NULL;
}

struct Hash *create_hash(int size) {

	struct Hash *hash = (struct Hash *) calloc(1, sizeof(struct Hash));
	if (hash == NULL) {
		perror("calloc");
		exit(1);
	}

	hash->hashtab = (struct Block **) calloc(1, sizeof(struct Block *) * size);
	if (hash->hashtab == NULL) {
		free(hash);
		perror("calloc");
		exit(1);
	}

	hash->size_hashtab = size;

	return hash;
}

char *add_hash(struct Hash *hash, char *key, char *value) {
	struct Block *findp;
	if ((findp = find_hash(hash, key)) != NULL) {
		return findp->value;
	}
	
	struct Block *p;
	p = (struct Block *) malloc(sizeof(struct Block));
	if (p == NULL) {
		perror("malloc");
		exit(1);
	}

	p->key = strdup(key);
	if (p->key == NULL) {
		perror("strdup");
		exit(1);
	}
	p->value = strdup(value);
	if (p->value == NULL) {
		perror("strdup");
		exit(1);
	}
	p->p = hash->hashtab[hashfunc(hash, key)];
	hash->hashtab[hashfunc(hash, key)] = p;
	
	return p->value;

}
