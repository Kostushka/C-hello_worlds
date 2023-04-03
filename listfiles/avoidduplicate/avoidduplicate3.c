#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ARGV_SIZE(x) (int) (sizeof(x) / sizeof(x[0]))

// Вариант исключения дублей при рекурсивном выводе файлов без доп. массивов, с заменой исключаемых путей на NULL (наиболее компактный способ)
// * все пути должны быть без / в конце

int is_prefix(char *curr, char *check);

int main(void) {
	// буфер путей
	char *argv[] = {"/top/dir1", "/top/dir1", "/top/dir1/subdir", "/dir2/dir1",
	                "/dir2", "/dir20/subdir", "/dir2/subdir/asdfsadf"};

	// кол-во исключаемых путей
	int count_null = 0;

	// сравниваем каждый путь с каждым кроме самого себя
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
		if (argv[i] == NULL) {
			continue;
		}
		for (int j = 0; j < ARGV_SIZE(argv); j++) {
			if (i == j) {
				continue;
			}
			if (argv[j] == NULL) {
				continue;
			}
			// проверяем, является curr - префиксом check
			if (is_prefix(argv[i], argv[j])) {
				// исключаем check
				argv[j] = NULL;				
				++count_null;
			}
		}
	}

	char **paths;

	// выделяем буфер для нужных путей
	if ((paths = (char **) malloc(sizeof(char *) * (ARGV_SIZE(argv) - count_null))) == NULL) {
		perror("malloc");
		return 1;
	}

	int k = 0;
	// проходимся по исходному буферу
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
		// если путь - не NULL
		if (argv[i] != NULL) {
			// записываем в итоговый буфер
			paths[k++] = argv[i];
		}
	}

	// вывод нужных путей
	for (int i = 0; i < ARGV_SIZE(argv) - count_null; i++) {
		printf("%s\n", paths[i]);
	}
	
	return 0;
}

// /top/dir1
// /top/dir1/subdir

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
