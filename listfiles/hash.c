#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listfilesnotbufreq.h"

static struct Hash *hash;

// по ключу (строке) получаем число (индекс в буфере)
unsigned hashfunc(char *s) {
	unsigned hashvalue;
	for (hashvalue = 0; *s != '\0'; s++) {
		hashvalue = *s + 31 * hashvalue;
	}
	return hashvalue % hash->size_hashtab;
}

int findhash(char *key, struct Block **hashtab) {
	struct Block *p;
	// в указатель на структуру получаем адрес структуры из хэша или NULL
	for (p = hashtab[hashfunc(key)]; p != NULL; p = p->p) {
		// если структура есть, проверяем соответствие ключей: ключи не равны ? идем дальше по связанному списку : возвращаем 1
		if (strcmp(p->key, key) == 0) {
			return p->value;
		}
	}

	// структура по ключу не найдена, возвращаем 0
	return 0;
}

int addhash(char *key, struct Hash **phash) {

	hash = *phash;
	
	if (hash == NULL) {
		if ((hash = (struct Hash *) malloc(sizeof(struct Hash))) == NULL) {
			perror("malloc");
			return -1;
		}
	}

	hash->size_hashtab = 10;
	hash->count_struct = 0;
	
	if (hash->count_struct == 0) {
		if ((hash->hashtab = (struct Block **) malloc(sizeof(struct Block *) * hash->size_hashtab)) == NULL) {
			perror("malloc");
			return -1;
		}
	}

	if (hash->count_struct == hash->size_hashtab) {
		hash->size_hashtab *= 2;
		if ((hash->hashtab = (struct Block **) realloc(hash->hashtab, sizeof(struct Block *) * hash->size_hashtab)) == NULL) {
			perror("realloc");
			return -1;
		}
	}
	
	// проверяем наличие в хэше структуры по ключу
	if (findhash(key, hash->hashtab)) {
		*phash = hash;
		// в хэше уже есть структура по данному ключу
		return 1;
	}

	struct Block *p;

	// выделяем память под новую структуру, записываем в указатель
	if ((p = (struct Block *) malloc(sizeof(struct Block))) == NULL) {
		perror("malloc");
		return -1;
	}
	
	hash->count_struct++;

	// выделяем память под ключ (строку)
	if ((p->key = (char *) malloc(strlen(key) + 1)) == NULL) {
		perror("malloc");
		return -1;
	}
	
	// записываем ключ в структуре
	strcpy(p->key, key);
	// записываем значение в структуре
	p->value = 1;
	// записываем указатель на предыдущую структуру, записанную по этому индексу в массив или NULL, если это первая запись 
	fprintf(stderr, "h1: %p key: %s\n", hash->hashtab[hashfunc(key)], key);
	p->p = hash->hashtab[hashfunc(key)];
	// записываем указатель на эту структуру в хэш по данному индексу
	hash->hashtab[hashfunc(key)] = p;
	fprintf(stderr, "h2: %p key: %s\n", hash->hashtab[hashfunc(key)], key);


	*phash = hash;
	
	return 0;
}
