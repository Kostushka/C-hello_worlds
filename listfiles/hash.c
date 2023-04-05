#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listfilesnotbufreq.h"

static struct Block **hashtab;

int size_hashtab = 10;
int count_struct = 0;

// по ключу (строке) получаем число (индекс в буфере)
unsigned hashfunc(char *s) {
	unsigned hashvalue;
	for (hashvalue = 0; *s != '\0'; s++) {
		hashvalue = *s + 31 * hashvalue;
	}
	return hashvalue % size_hashtab;
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

int addhash(struct Block ***phashtab, char *key) {
	struct Block *p;

	hashtab = *phashtab; 

	if (count_struct == 0) {
		if ((hashtab = (struct Block **) malloc(sizeof(struct Block *) * size_hashtab)) == NULL) {
			perror("malloc");
			return -1;
		}
	}

	if (count_struct == size_hashtab) {
		size_hashtab *= 2;
		if ((hashtab = (struct Block **) realloc(hashtab, sizeof(struct Block *) * size_hashtab)) == NULL) {
			perror("realloc");
			return -1;
		}
	}
	
	// проверяем наличие в хэше структуры по ключу
	if (findhash(key, hashtab)) {
		// в хэше уже есть структура по данному ключу
		return 1;
	}
	// выделяем память под новую структуру, записываем в указатель
	if ((p = (struct Block *) malloc(sizeof(struct Block))) == NULL) {
		perror("malloc");
		return -1;
	}
	
	++count_struct;
	
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
	p->p = hashtab[hashfunc(key)];
	// записываем указатель на эту структуру в хэш по данному индексу
	hashtab[hashfunc(key)] = p;

	*phashtab = hashtab;
	
	return 0;
}
