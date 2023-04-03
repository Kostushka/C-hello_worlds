#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listfilesnotbufreq.h"

struct Block *hashtab[1000];

// по ключу (строке) получаем число (индекс в буфере)
unsigned hashfunc(char *s) {
	unsigned hashvalue;
	for (hashvalue = 0; *s != '\0'; s++) {
		hashvalue = *s + 31 * hashvalue;
	}
	return hashvalue % 1000;
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

int addhash(struct Block **hashtab, char *key) {
	struct Block *p;
	// проверяем что в хэше нет структуры по ключу
	if (findhash(key, hashtab) == 0) {
		// выделяем память под новую структуру, записываем в указатель
		if ((p = (struct Block *) malloc(sizeof(struct Block))) == NULL) {
			perror("malloc");
			return -1;
		}
		// выделяем память под ключ (строку)
		if ((p->key = (char *) malloc(strlen(key) + 1)) == NULL) {
			perror("malloc");
			return -1;
		}
		// записываем ключ в структуре
		strcpy(p->key, key);
		// записываем значение в структуре
		p->value = 1;
		// записываем ссылку на другую структуру
		p->p = NULL;
		// записываем указатель на структуру в хэш
		hashtab[hashfunc(key)] = p;
		return 0;
	} else {
	// в хэше уже есть структура по ключу
		return 1;
	}
}
