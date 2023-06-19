#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "labyrinth.h"

struct Labyrinth *init_labyrint(int fd, int file_size) {
	struct Labyrinth *lab = (struct Labyrinth *) malloc(sizeof(struct Labyrinth));
	lab->traveler.x = -1;
	lab->traveler.y = -1;
	lab->target.x = -1;
	lab->target.y = -1;

	char line[10];

	// читаю первую строку файла
	if ((get_string(fd, line, sizeof(line))) == NULL) {
		return NULL;
	}

	// записываю размер лабиринта
	if (sscanf(line, "%d", &lab->size) != 1) {
		fprintf(stderr, "Invalid string: %s\n", line);
		return NULL;
	}

	// выделяю память под массив указателей на массивы строк == строки двумерного массива
	// {p, p, p, p}
	lab->labyrinth = (char **) calloc(lab->size, sizeof(char *));
	if (lab->labyrinth == NULL) {
		perror("calloc");
		return NULL;
	}

	// создаю двумерный массив
	if (load_labyrinth(lab, fd, file_size) == NULL) {
		destroy_lab(lab);
		return NULL;
	}

	return lab;
}

struct Labyrinth *load_labyrinth(struct Labyrinth *lab, int fd, int file_size) {
	char *line;

	int i;
	// формирую двумерный массив
	for (i = 0; i < lab->size; i++) {
		// читаю строку из файла
		line = get_row(fd, lab->size, i, &lab->traveler, &lab->target);

		// проверка строки на EOF: файл меньше заданной размерности
		if (line == NULL) {
			fprintf(stderr, "file size is less than matrix size\n");
			return NULL;
		}

		// записываю в указатель строку == столбцы двумерного массива
		// {p, p, p, p}: p -> {'a', 'b', 'c'}
		lab->labyrinth[i] = line;
	}

	// проверка, что координаты '*' получены
	if (lab->traveler.x == -1) {
		fprintf(stderr, "file incorrect: missing symbol '*'\n");
		return NULL;
	}

	// проверка, что координаты '+' получены
	if (lab->target.x == -1) {
		fprintf(stderr, "file incorrect: missing symbol '+'\n");
		return NULL;
	}

	// позиция в файле после прочтения
	int pos = lseek(fd, 0, SEEK_CUR);
	if (pos == -1) {
		perror("lseek");
		return NULL;
	}
	if (pos != file_size) {
		fprintf(stderr, "file size is larger than matrix size\n");
		return NULL;
	}

	return lab;
}

