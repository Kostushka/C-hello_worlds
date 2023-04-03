#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ARGV_SIZE(x) (int) (sizeof(x) / sizeof(x[0]))

// Вариант исключения дублей при рекурсивном выводе файлов с применением нормализации путей и с использованием буфера флагов
// * все пути должны быть без / в конце

int is_prefix(char *curr, char *check);

int main(void) {
	// буфер с путями
	char *argv[] = {"/top/dir1", "/top/dir1", "/top/dir1/subdir", "/dir2/dir1",
	                "/dir2", "/dir20/subdir", "/dir2/subdir/asdfsadf"};

	// нормализация - добавляем всем путям / в конце
	// для каждого пути в исходном буфере
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
		char *path;
		// выделяем память, так как строку по указателю менять нельзя
		if ((path = (char *) malloc(strlen(argv[i] + 2))) == NULL) {
			perror("malloc");
			return 1;
		}
		// копируем в выделенную память путь
		strcpy(path, argv[i]);
		// добавляем / в конец пути
		strcat(path, "/");
		// перезаписываем исходный указатель на указатель на строку со / в конце
		// /top/dir -> /top/dir/
		argv[i] = path;
	}

	// буфер флагов
	char nums[ARGV_SIZE(argv)];
	// инициализация буфера
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
		nums[i] = 0;
	}

	// кол-во искл. путей
	int count_paths = 0;

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
			if (is_prefix(argv[i], argv[j])) {
				if (strcmp(argv[i], argv[j]) == 0) {
					argv[j] = NULL;
				}
				nums[j] = 1;
				++count_paths;
			}
		}
	}

	char **paths;

	// память для нового буфера
	if ((paths = (char **) malloc(sizeof(char *) * (ARGV_SIZE(argv) - count_paths))) == NULL) {
		perror("malloc");
		return 1;
	}

	// проходимся по буферу флагов, записываем пути, которым соответсвует значение 0
	int k = 0;
	for (int i = 0; i < ARGV_SIZE(argv); i++) {
		if (nums[i] == 0) {
			paths[k++] = argv[i];
		}
	}

	// вывод нужных путей
	for (int i = 0; i < ARGV_SIZE(argv) - count_paths; i++) {
		printf("%s\n", paths[i]);
	}
	
	return 0;
}

// /run1/
// /run12/

int is_prefix(char *curr, char *check) {

	// curr - префикс check ? 1 : 0
	while (*curr == *check && *curr != '\0') {
		++curr;
		++check;
	}
	if (*curr == '\0') {
		return 1;
	}
	return 0;
}
