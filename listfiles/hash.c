#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listfilesnotbufreq.h"

// по ключу (строке) получаем число (индекс в буфере)
unsigned hashfunc(struct Hash *hash, char *s) {
	unsigned hashvalue;
	for (hashvalue = 0; *s != '\0'; s++) {
		hashvalue = *s + 31 * hashvalue;
	}
	return hashvalue % hash->size_hashtab;
}

int findhash(struct Hash *hash, char *key) {
	struct Block *p;
	// в указатель на структуру получаем адрес структуры из хэша или NULL
	for (p = hash->hashtab[hashfunc(hash, key)]; p != NULL; p = p->p) {
		// если структура есть, проверяем соответствие ключей: ключи не равны ? идем дальше по связанному списку : возвращаем 1
		if (strcmp(p->key, key) == 0) {
			return p->value;
		}
	}

	// структура по ключу не найдена, возвращаем 0
	return 0;
}

// дает возможность создать много хэшей
struct Hash *createhash(int size) {

	// выделяем память под структуру хэша
	struct Hash *hash = (struct Hash *) malloc(sizeof(struct Hash));
	if (hash == NULL) {
		perror("malloc");
		return NULL;
	}

	// выделяем память под массив структур
	if ((hash->hashtab = (struct Block **) malloc(sizeof(struct Block *) * size)) == NULL) {
		free(hash);
		perror("malloc");
		return NULL;
	}

	hash->size_hashtab = size;
	hash->count_struct = 0;

	return hash;
} 

int addhash(struct Hash *hash, char *key) {
	
	// if (hash->count_struct == hash->size_hashtab) {
		// hash->size_hashtab *= 2;
		// if ((hash->hashtab = (struct Block **) realloc(hash->hashtab, sizeof(struct Block *) * hash->size_hashtab)) == NULL) {
			// perror("realloc");
			// return -1;
		// }
	// }
	
	// проверяем наличие в хэше структуры по ключу
	if (findhash(hash, key)) {
		// в хэше уже есть структура по данному ключу
		return 1;
	}

	struct Block *p;

	// выделяем память под новую структуру, записываем в указатель
	if ((p = (struct Block *) malloc(sizeof(struct Block))) == NULL) {
		perror("malloc");
		return -1;
	}

	// инкрементируем счетчик структур
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
	p->p = hash->hashtab[hashfunc(hash, key)];
	// записываем указатель на эту структуру в хэш по данному индексу
	hash->hashtab[hashfunc(hash, key)] = p;
	
	return 0;
}
