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
	if (argc != 3) {
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
	printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
	printf("+: {%d; %d}\n", lab->target.x, lab->target.y);

	// получить размер файла с командами
	struct stat stbuf_command;
	if (stat(argv[2], &stbuf_command) == -1) {
		fprintf(stderr, "%s: %s\n", argv[2], strerror(errno));
	}

	// открыть файл с командами
	if ((fd = open(argv[2], O_RDONLY, 0)) == -1) {
		fprintf(stderr, "%s: %s\n", argv[2], strerror(errno));
	}

	struct Command *command;
	int pos = 0;
	int success = 0;

	while (pos != stbuf_command.st_size) {		
		// функция парсинга строки из файла команд в структуру для команды
		if ((command = init_command(fd)) == NULL) {
			destroy_lab(lab);
			return 1;
		}
		
		int flag;
		switch(command->flag) {
			case 'U': 
				flag = UP;
				break;
			case 'D':
				flag = DOWN;
				break;
			case 'L':
				flag = LEFT;
				break;
			case 'R':
				flag = RIGHT;
				break;
		}
	
		// перемещение по лабиринту
		while (command->num > 0){
			if (move(lab, flag) != 0) {
				free(command);
				destroy_lab(lab);
				return 1;
			}
			if (lab->traveler.x == lab->target.x && lab->traveler.y == lab->target.y) {
				success = 1;
			}
			--command->num;
		}
	
		// отрисовать лабиринт
		print_lab(lab);
		printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
		printf("+: {%d; %d}\n", lab->target.x, lab->target.y);
		
		pos = lseek(fd, 0, SEEK_CUR);
		if (pos == -1) {
			destroy_lab(lab);
			free(command);
			perror("lseek");
			return 1;
		}
		// очистить память, выделенную под структуру команд
		free(command);
	}
	if (!success) {
		printf("FAIL!\n");
	} else {
		printf("SUCCESS!\n");
	}

	// закрыть файл с командами
	close(fd);
	
	// очистить память, выделенную под лабиринт
	destroy_lab(lab);

	return 0;
}

struct Command *init_command(int fd) {
	// создать структуру для команды
	struct Command *command = (struct Command *) malloc(sizeof(struct Command));
	command->num = -1;
	command->flag = -1;

	// распарсить строку из файла команд в структуру для команды
	char buf;
	int i, n;
	i = n = 0;
	while ((n = read(fd, &buf, 1)) != 0) {
		if (n == -1) {
			free(command);
			perror("read");
			return NULL;
		}
		if (buf == '\n') {
			break;
		}
		if (buf == ' ') {
			continue;
		}
		if (i == 0) {
			command->num = buf - '0';
		} else {
			command->flag = buf;
		}
		++i;
	}

	if (command->num == -1 || command->flag == -1) {
		free(command);
		return NULL;
	}

	return command;
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


