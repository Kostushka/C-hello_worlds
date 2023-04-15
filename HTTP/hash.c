#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsinghttp.h"

unsigned hash_func(struct Hash *hash, char *key) {
	unsigned hashvalue;
	for (hashvalue = 0; *key != '\0'; key++) {
		hashvalue = *key + 31 * hashvalue;
	}
	return hashvalue % hash->size_hashtab;
}

struct Block *hash_find(struct Hash *hash, char *key) {
	struct Block *p;
	for (p = hash->hashtab[hash_func(hash, key)]; p != NULL; p = p->p) {
		if (strcmp(p->key, key) == 0) {
			return p;
		}
	}
	return NULL;
}

struct Hash *hash_create(int size) {

	struct Hash *hash = (struct Hash *) calloc(1, sizeof(struct Hash));
	if (hash == NULL) {
		perror("calloc");
		return NULL;
	}

	hash->hashtab = (struct Block **) calloc(1, sizeof(struct Block *) * size);
	if (hash->hashtab == NULL) {
		free(hash);
		perror("calloc");
		return NULL;
	}

	hash->size_hashtab = size;

	return hash;
}

char *hash_add(struct Hash *hash, char *key, char *value) {
	struct Block *findp;
	if ((findp = hash_find(hash, key)) != NULL) {
		free(findp->value);
		findp->value = strdup(value);
		if (findp->value == NULL) {
			perror("strdup");
			return NULL;
		}
		return findp->value;
	}
	
	struct Block *p;
	p = (struct Block *) malloc(sizeof(struct Block));
	if (p == NULL) {
		perror("malloc");
		return NULL;
	}

	p->key = strdup(key);
	if (p->key == NULL) {
		perror("strdup");
		return NULL;
	}
	p->value = strdup(value);
	if (p->value == NULL) {
		perror("strdup");
		return NULL;
	}
	p->p = hash->hashtab[hash_func(hash, key)];
	hash->hashtab[hash_func(hash, key)] = p;
	
	return p->value;

}

char *hash_get(struct Hash *hash, char *key) {
	struct Block *p;
	if ((p = hash_find(hash, key)) != NULL) {
		return p->value;
	}
	return NULL;
}
