#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listfilesnotbufreq.h"

struct Block *hashtab[1000];

unsigned hashfunc(char *s) {
	unsigned hashvalue;
	for (hashvalue = 0; *s != '\0'; s++) {
		hashvalue = *s + 31 * hashvalue;
	}
	return hashvalue % 100;
}

int findhash(char *key, struct Block **hashtab) {
	struct Block *p;
	for (p = hashtab[hashfunc(key)]; p != NULL; p = p->p) {
		if (strcmp(p->key, key) == 0) {
			return p->value;
		}
	}
	return 0;
}

int addhash(struct Block **hashtab, char *key) {
	struct Block *p;
	if (findhash(key, hashtab) == 0) {
		if ((p = (struct Block *) malloc(sizeof(struct Block *))) == NULL) {
			perror("malloc");
			return -1;
		}
		if ((p->key = (char *) malloc(sizeof(char *))) == NULL) {
			perror("malloc");
			return -1;
		}
		strcpy(p->key, key);
		p->value = 1;
		p->p = NULL;	
		hashtab[hashfunc(key)] = p;
		return 0;
	} else {
		return 1;
	}
}
