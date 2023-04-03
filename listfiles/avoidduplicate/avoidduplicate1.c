#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ARGV_SIZE(x) (int) (sizeof(x) / sizeof(x[0]))

// Вариант исключения дублей при рекурсивном выводе файлов с использованием буфера флагов
// * все пути должны быть без / в конце

int is_prefix(char *curr, char *check);

int main(void) {
	// массив путей
	char *argv[] = {"/run", "/home/run/dev", "/run/", "/home/run", "/home12/d1", "/home12/d2"};

	// массив флагов: [0, 1, 1, 0, ...] - 0 нужный путь, 1 исключаемый путь
	char nums[ARGV_SIZE(argv)];

	// инициализируем все элементы массива флагов 0
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
		nums[i] = 0;
	}

	// кол-во исключаемых путей
	int count_paths = 0;

	// сравниваем каждый путь со всеми кроме самого себя
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
	
		char *curr = argv[i];
		
		// проверяем, что путь не NULL
		if (curr == NULL) {
			continue;
		}
		
		for (int j = 0; j < ARGV_SIZE(argv); j++) {
			if (i == j) {
				continue;
			}
			
			char *check = argv[j];
			
			// проверяем, что путь не NULL
			if (check == NULL) {
				continue;
			}
			
			/* проверяем, является ли curr - префиксом check
			   если является, значит check является подкаталогом curr 
			// проверяем, является ли /dir1/ префиксом /dir1/subdir,
            // если является, значит /dir1/subdir является подкаталогом /dir1/
            */
			if (is_prefix(curr, check)) {
				// проверяем, совпадают ли curr и check
				if (strcmp(curr, check) == 0) {
					// check присваиваем NULL, чтобы пути не взаимоисключили друг друга
					// используем argv[j], так как check - указатель на указатель 
					argv[j] = NULL;
				}
				// исключаем check в массиве флагов
				nums[j] = 1;
				// инкрементируем счетчик исключаемых путей
				++count_paths;
			}
		}
	}

	char **paths;

	// выделяем память под массив указателей длинной: кол-во всех путей - кол-во исключаемых путей
	if ((paths = (char **) malloc(sizeof(char *) * (ARGV_SIZE(argv) - count_paths))) == NULL) {
		perror("malloc");
		return 1;
	}

	// пробегаемся по массиву флагов, если не 1, добавляем путь в итоговый массив
	int k = 0;
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
		if (nums[i] == 0) {
			paths[k] = argv[i];
			printf("%s\n", paths[k]); 
			++k;
		}
	}
	
	return 0;
}

// /run1
// /run1

int is_prefix(char *curr, char *check) {

	// curr - префикс check ? 1 : 0
	while (*curr == *check && *curr != '\0') {
		++curr;
		++check;
	}
	if (*curr == '\0' && (*check == '/' || *check == '\0')) {
		return 1;
	}
	
	return 0;
}
