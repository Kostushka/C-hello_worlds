#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "labyrinth.h"

int main(int argc, char *argv[]) {	
	int fd;

	// проверить корректность параметров программы
	if (argc != 3) {
		printf("incorrect input of program args\n");
		return 1;
	}

	// получить размер файла
	struct stat stbuf;
	if (stat(argv[1], &stbuf) == -1) {
		fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
		return 1;
	}

	// открыть файл конфигурации
	if ((fd = open(argv[1], O_RDONLY, 0)) == -1) {
		fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
		return 1;
	}
	
	struct Labyrinth *lab;
	// функция парсинга текстового файла
	if ((lab = init_labyrint(fd, stbuf.st_size)) == NULL) {
		destroy_lab(lab);
		close(fd);
		return 1;
	}

	// закрыть файл конфигурации
	close(fd);

	// отрисовать лабиринт
	print_lab(lab);
	printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
	printf("+: {%d; %d}\n", lab->target.x, lab->target.y);

	FILE *fp;
	// открыть файл с командами
	if ((fp = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "%s: %s\n", argv[2], strerror(errno));
		destroy_lab(lab);
		return 1;
	}

	// функция обработки файла команд
	if (load_command(fp, lab) == NULL) {
		destroy_lab(lab);
		fclose(fp);
		return 1;
	}

	// закрыть файл с командами
	fclose(fp);
	
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


