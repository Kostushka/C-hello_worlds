#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "listfilesnotbufreq.h"

int includePath(char *curr, char *check);

int getNotDuplPaths(char **argv, int len, char ***paths) {

	// с помощью  бибилиотечной realpath() получаем для каждого аргумента полный путь от /
	for (int i = 0; i < len; i++) {
		argv[i] = realpath(argv[i], NULL);
	}

	// если один аргумент
	if (len == 1) {
		if ((*paths = (char **) malloc(sizeof(char *))) == NULL) {
			perror("malloc");
			exit(1);
		}
		// записываем исходный массив с путем
		*paths = argv;
		// кол-во путей: 1
		return 1;
	}

	// кол-во путей, замененных на NULL
	int countNull = 0;

	// каждый путь сравниваем со всеми кроме самого себя
	for (int i = 0; i < len; i++) {
		// учет путей, замененных на NULL
		if (argv[i] == NULL) {
			continue;
		}
		for (int j = 0; j < len; j++) {
			if (i == j) {
				continue;
			}
			// учет путей, замененных на NULL
			if (argv[j] == NULL) {
				continue;
			}
			// если curr является префиксом check или пути соответсвуют друг другу
			if (includePath(argv[i], argv[j])) {
				// заменяем check на NULL - то есть исключаем этот путь
				argv[j] = NULL;
				// инкрементируем счетчик NULL
				++countNull;
			}
		}
	}

	// выделяем память для массива указателей нужных нам путей (длина: кол-во всех путей - кол-во NULL)
	if ((*paths = (char **) malloc(sizeof(char *) * (len - countNull))) == NULL) {
		perror("malloc");
		exit(1);
	}

	int k = 0;
	// проходимся по всему исходному массиву путей
	for (int i = 0; i < len; i++) {
		// записываем пути в новый массив указателей
		if (argv[i] != NULL) {
			(*paths)[k++] = argv[i];
		}
	}
	
	// возвращаем длину конечного массива нужных путей 
	return len - countNull;
}

// /home
// /home12

int includePath(char *curr, char *check) {
	// доходим до конца curr или до несовпадения curr и check
	while (*curr == *check && *curr != '\0') {
		++curr;
		++check;
	}
	// если дошли до конца curr и сheck - '\0' или /, то сurr - префикс check
	if (*curr == '\0' && (*check == '\0' || *check == '/')) {
		return 1;
	}
	// curr - не префикс check
	return 0;
}
