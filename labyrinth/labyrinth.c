#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct Labyrinth {
	int size;
	char **labyrinth;
};

int parse(struct Labyrinth *, int fd);
char *get_line(int fd, int size);
void print_lab(struct Labyrinth);
char **create_arr(struct Labyrinth *, int fd);

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
	int size = lab.size;
	while (size > 0) {
		if (size == lab.size) {
			printf("+");
		} 
		printf("-");
		if (size == 1) {
			printf("+");
		}
		--size;
	}
	putchar('\n');
	for (int i = 0; i < lab.size; i++) {
		printf("|");
		for (int j = 0; j < lab.size; j++) {
			printf("%c", lab.labyrinth[i][j]);
		}
		printf("|");
		putchar('\n');
	}
	size = lab.size;
	while (size > 0) {
		if (size == lab.size) {
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

int parse(struct Labyrinth *lab, int fd) {

	char *line;
	
	// читаю первую строку файла
	if ((line = get_line(fd, 10)) == NULL) {
		return -1;
	}
	// записываю размер лабиринта
	lab->size = atoi(line);

	// создаю двумерный массив
	if (create_arr(lab, fd) == NULL) {
		return -1;
	}
	
	return 0;
}

char **create_arr(struct Labyrinth *lab, int fd) {

	char *line;
	
	// выделяю память под массив указателей на массивы строк == строки двумерного массива
	// {p, p, p, p}
	lab->labyrinth = (char **) calloc(lab->size, sizeof(char *));
	if (lab->labyrinth == NULL) {
		perror("calloc");
		return NULL;
	}

	int i;
	// формирую двумерный массив
	for (i = 0; i < lab->size; i++) {
		if ((line = get_line(fd, lab->size)) == NULL) {
			return NULL;
		}
		// записываю в указатель строку == столбцы двумерного массива
		// {p, p, p, p}: p -> {'a', 'b', 'c'}
		lab->labyrinth[i] = line;
	}

	return lab->labyrinth;
}

char *get_line(int fd, int size) {
	// кол-во считанных из файла строк
	static int len = 0;
	
	char *s = (char *) calloc(size, sizeof(char));
	if (s == NULL) {
		perror("calloc");
		return NULL;
	}
	
	int i, c;
	i = c = 0;
	char buf;
	
	// читать по символу из файла в отдельный буфер
	while ((c = read(fd, &buf, 1)) != 0) {
		if (c == -1) {
			perror("read");
			return NULL;
		}
		if (buf == '\n') {
			break;
		}
		if (i == size) {
			fprintf(stderr, "line number %d: not enough space to write\n", len + 1);
			return NULL;
		}				
		s[i++] = buf;
	}

	// обработка файла, который меньше заданной размерности
	if (c == 0 && len < size + 1) { // size + 1 там должен находиться EOF
		fprintf(stderr, "file size is less than matrix size\n");
		return NULL;
	}

	// обработка файла, который больше заданной размерности
	if (len == size && read(fd, &buf, 1) != 0) {
		fprintf(stderr, "file size is larger than matrix size\n");
		return NULL;
	}
	++len;

	while (i < size) {
		s[i++] = ' ';
	}
	
	return s;
}
