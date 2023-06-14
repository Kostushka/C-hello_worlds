#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

struct Point {
	int x;
	int y;
};

struct Labyrinth {
	int size;
	char **labyrinth;
	struct Point point;
};

int parse(struct Labyrinth *, int fd, int file_size);
char *get_row(int fd, int size, int num_line, struct Point *);
void print_lab(struct Labyrinth);
char **create_arr(struct Labyrinth *, int fd, int file_size);
void destroy_lab(struct Labyrinth *);
struct Point *get_point(struct Point *, int c, int num_line, int num_c);

int main(int argc, char *argv[]) {
	struct Labyrinth lab;
	lab.point.x = -1;
	lab.point.y = -1;
	
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

	// функция парсинга текстового файла
	if (parse(&lab, fd, stbuf.st_size) < 0) {
		return 1;
	}

	// закрыть файл конфигурации
	close(fd);

	// отрисовать лабиринт
	print_lab(lab);

	printf("*: {%d; %d}\n", lab.point.x, lab.point.y);

	// очистить память, выделенную под лабиринт
	destroy_lab(&lab);
	
	return 0;
}

void destroy_lab(struct Labyrinth *lab) {
	for (int k = 0; k < lab->size && lab->labyrinth[k] != NULL; k++) {
		free(lab->labyrinth[k]);
	}
	free(lab->labyrinth);
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

int parse(struct Labyrinth *lab, int fd, int file_size) {

	char *line;
	
	// читаю первую строку файла
	if ((line = get_row(fd, 10, 0, &lab->point)) == NULL) {
		return -1;
	}
	// записываю размер лабиринта
	lab->size = atoi(line);
	free(line);
	
	// создаю двумерный массив
	if (create_arr(lab, fd, file_size) == NULL) {
		return -1;
	}
	
	return 0;
}

char **create_arr(struct Labyrinth *lab, int fd, int file_size) {

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
		// читаю строку из файла
		line = get_row(fd, lab->size, i, &lab->point);
		
		// проверка строки на EOF: файл меньше заданной размерности
		if (line == NULL) {
			fprintf(stderr, "file size is less than matrix size\n");
			destroy_lab(lab);
			return NULL;
		}

		// записываю в указатель строку == столбцы двумерного массива
		// {p, p, p, p}: p -> {'a', 'b', 'c'}
		lab->labyrinth[i] = line;
	}
	
	// проверка, что координаты '*' получены 
	if (lab->point.x == -1) {
		fprintf(stderr, "file incorrect: missing symbol '*'\n");
		destroy_lab(lab);
		return NULL;
	}

	// позиция в файле после прочтения
	int pos = lseek(fd, 0, SEEK_CUR);
	if (pos == -1) {
		perror("lseek");
	}
	if (pos != file_size) {
		fprintf(stderr, "file size is larger than matrix size\n");
		destroy_lab(lab);
		return NULL;
	}
	
	return lab->labyrinth;
}

char *get_row(int fd, int size, int num_line, struct Point *point) {
	char *s = (char *) calloc(size, sizeof(char));
	if (s == NULL) {
		perror("calloc");
		return NULL;
	}
	
	int i, c;
	i = c = 0;
	char buf = 0;
	
	// читать по символу из файла в отдельный буфер
	while ((c = read(fd, &buf, 1)) != 0) {
		if (c == -1) {
			free(s);
			perror("read");
			return NULL;
		}
		if (buf == '\n') {
			break;
		}
		if (i == size) {
			free(s);
			// num_line + 2 - отсчет с 0 и 1-ая строка содержит служебную информацию 
			fprintf(stderr, "line number %d: line too long\n", num_line + 2);
			return NULL;
		}
				
		// получить координаты '*' или NULL, если координаты уже были получены
		if (get_point(point, buf, num_line, i) == NULL) {
			return NULL;
		}
				
		s[i++] = buf;
	}

	// провекра, что EOF и в s ничего не записано, так как не во всех файлах есть \n в конце любой строки
	if (c == 0 && s[0] == 0) {
		fprintf(stderr, "unexpected EOF\n");
		return NULL;
	}

	while (i < size) {
		s[i++] = ' ';
	}
	
	return s;
}

struct Point *get_point(struct Point *point, int c, int num_line, int num_c) {
	if (c != '*') {
		return point;
	}
	// проверка, что в файле уже записаны координаты '*'
	if (point->x != -1) {
		// num_line + 2 - отсчет с 0 и 1-ая строка содержит служебную информацию 
		fprintf(stderr, "file incorrect: more than one '*' in file, second '*' in %d line\n", num_line + 2);
		return NULL;
	}
	point->x = num_line;
	point->y = num_c;
	return point;
}
