#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAX 100

struct Labyrinth {
	int size;
	char labyrinth[MAX][MAX];
};

int parse(struct Labyrinth *, int fd);
int get_line(int fd, char *s, int lim);
void print_lab(struct Labyrinth);

int main(int argc, char *argv[]) {
	struct Labyrinth lab;
	int fd;

	// проверить корректность параметров программы
	if (argc != 2) {
		printf("incorrect input of program args\n");
		return 1;
	}

	// открыть файл конфигурации
	if ((fd = open(argv[1], O_RDONLY, 0)) == -1) {
		fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
	}

	// функция парсинга текстового файла
	if (parse(&lab, fd) < 0) {
		return 1;
	}

	// закрыть файл конфигурации
	close(fd);

	// отрисовать лабиринт
	print_lab(lab);
	
	return 0;
}

void print_lab(struct Labyrinth lab) {
	for (int i = 0; i < lab.size; i++) {
		for (int j = 0; j < lab.size; j++) {
			printf("%c", lab.labyrinth[i][j]);
		}
		putchar('\n');
	}
}

int parse(struct Labyrinth *lab, int fd) {
	char line[MAX];
	// читаю первую строку файла
	if (get_line(fd, line, MAX) == -1) {
		return -1;
	}
	// записываю размер лабиринта
	lab->size = atoi(line);

	int i, j;
	// формирую двумерный массив
	for (i = 0; i < lab->size; i++) {
			if (get_line(fd, line, MAX) == -1) {
				return -1;
			}
			for (j = 0; j < lab->size; j++) {
				if (line[j] == '\0') {
					break;
				} else {
					lab->labyrinth[i][j] = line[j];
				}
			}
			while (j < lab->size) {
				lab->labyrinth[i][j] = ' ';
				++j;
			}
	}
	return 0;
}

int get_line(int fd, char *s, int lim) {
	int i, c;
	i = c = 0;
	char buf;
	
	while (i < lim - 1) {
		// читать по символу из файла в отдельный буфер
		if ((c = read(fd, &buf, 1)) == 0 || c == -1) {
			break;
		}
		if (buf == '\n') {
			break;
		}		
		s[i] = buf;
		++i;
	}

	s[i] = '\0';

	if (c == -1) {
		fprintf(stderr, "read file error\n");
		return -1;
	}
	return c;
}
