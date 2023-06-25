#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "labyrinth.h"

struct Hash *hash_create(int size) {
	// выделить память под структуру хэша
	struct Hash *hash = (struct Hash *) malloc(sizeof(struct Hash));
	if (hash == NULL) {
		perror("malloc");
		return NULL;
	}
	// выделить память под массив укзателей на структуры
	hash->hashtab = (struct Block **) calloc(size, sizeof(struct Block *));
      	if (hash->hashtab == NULL) {
		perror("calloc");
		free(hash);
		return NULL;
	}
	
	hash->count_struct = 0;
	hash->size_hashtab = size;
	
	return hash;	
}

unsigned hashfunc(struct Hash *hash, char *key) {
	unsigned hashvalue;
	for (hashvalue = 0; *key != '\0'; key++) {
		hashvalue = *key + 31 * hashvalue; 
	}
	return hashvalue % hash->size_hashtab;
}

command_handler hash_find(struct Hash *hash, char *key) {
	struct Block *p;
	for (p = hash->hashtab[hashfunc(hash, key)]; p != NULL; p = p->next) {
		// если команда с таким именем есть
		if (strcmp(p->key, key) == 0) {
			// функция обработчик команды
			return p->value;
		}
	}
	return NULL;	
}

int hash_add(struct Hash *hash, char *key, command_handler value) {
	// команда уже записана
	if (hash_find(hash, key) != NULL) {
		fprintf(stderr, "command %s is already added\n", key);
		return 1;
	}
	
	struct Block *command = (struct Block *) malloc(sizeof(struct Block));
	if (command == NULL) {
		perror("malloc");
		return 1;
	}

	hash->count_struct++;

	command->key = strdup(key);
	if (command->key == NULL) {
		perror("strdup");
		return 1;
	}
	command->value = value;
	command->next = hash->hashtab[hashfunc(hash, key)];
	hash->hashtab[hashfunc(hash, key)] = command;

	return 0;
}

