#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "labyrinth.h"

#define UP 		1 	//0001 x ^ -
#define DOWN 	2 	//0010 x v +
#define LEFT 	4 	//0100 y < -
#define RIGHT 	8 	//1000 y > +

int main(int argc, char *argv[]) {	
	int fd;

	// проверить корректность параметров программы
	if (argc != 2) {
		printf("incorrect input of program args\n");
		return 1;
	}

	// получить размер файла
	struct stat stbuf;
	if (stat(argv[1], &stbuf) == -1) {
		fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
	}

	// открыть файл конфигурации
	if ((fd = open(argv[1], O_RDONLY, 0)) == -1) {
		fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
	}
	
	struct Labyrinth *lab;
	// функция парсинга текстового файла
	if ((lab = init_labyrint(fd, stbuf.st_size)) == NULL) {
		return 1;
	}

	// закрыть файл конфигурации
	close(fd);

	// отрисовать лабиринт
	print_lab(lab);
	printf("*: {%d; %d}\n", lab->point.x, lab->point.y);

	// перемещение по лабиринту
	if (move(lab, RIGHT) != 0) {
		destroy_lab(lab);
		return 1;
	}
	// отрисовать лабиринт
	print_lab(lab);
	printf("*: {%d; %d}\n", lab->point.x, lab->point.y);

	// очистить память, выделенную под лабиринт
	destroy_lab(lab);

	return 0;
}

void destroy_lab(struct Labyrinth *lab) {
	for (int k = 0; k < lab->size && lab->labyrinth[k] != NULL; k++) {
		free(lab->labyrinth[k]);
	}
	free(lab->labyrinth);
	free(lab);
}

void print_lab(struct Labyrinth *lab) {
	int size = lab->size;
	while (size > 0) {
		if (size == lab->size) {
			printf("+");
		} 
		printf("-");
		if (size == 1) {
			printf("+");
		}
		--size;
	}
	putchar('\n');
	for (int i = 0; i < lab->size; i++) {
		printf("|");
		for (int j = 0; j < lab->size; j++) {
			printf("%c", lab->labyrinth[i][j]);
		}
		printf("|");
		putchar('\n');
	}
	size = lab->size;
	while (size > 0) {
		if (size == lab->size) {
			printf("+");
		} 
		printf("-");
		if (size == 1) {
			printf("+");
		}
		--size;
	}
	putchar('\n');
}


