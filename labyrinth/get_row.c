#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "labyrinth.h"

char *get_string(int fd, char *s, int n) {
	char buf;
	int c, i;
	i = 0;

	while ((c = read(fd, &buf, 1)) != 0) {
		if (c == -1) {
			perror("read");
			return NULL;
		}
		if (i == n) {
			fprintf(stderr, "file incorrect\n");
			return NULL;
		}
		if (buf == '\n') {
			s[i] = '\0';
			break;
		}
		s[i++] = buf;
	}

	if (c == 0) {
		fprintf(stderr, "file incorrect\n");
		return NULL;
	}

	return s;
}

char *get_row(int fd, int size, int num_line, struct Context *context) {
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

		// получить координаты '*' и '+' или NULL, если координаты уже были получены
		if (get_point(context, buf, num_line, i) == NULL) {
			free(s);
			return NULL;
		}

		s[i++] = buf;
	}

	// проверка, что EOF и в s ничего не записано, так как не во всех файлах есть \n в конце любой строки
	if (c == 0 && i == 0) {
		fprintf(stderr, "unexpected EOF\n");
		return NULL;
	}

	while (i < size) {
		s[i++] = ' ';
	}

	return s;
}

struct Point *get_point(struct Context *context, int c, int num_line, int num_c) {
	if (c != '*' && c != '+') {
		return &context->traveler;
	}
	
	if (c == '*') {
		// проверка, что в файле уже записаны координаты '*'
		if (context->traveler.x != -1) {
			// num_line + 2 - отсчет с 0 и 1-ая строка содержит служебную информацию
			fprintf(stderr, "file incorrect: more than one '*' in file, second '*' in %d line\n", num_line + 2);
			return NULL;
		}
		context->traveler.x = num_c;
		context->traveler.y = num_line;
		return &context->traveler;
	}
	
	// проверка, что в файле уже записаны координаты '+'
	if (context->target.x != -1) {
		// num_line + 2 - отсчет с 0 и 1-ая строка содержит служебную информацию
		fprintf(stderr, "file incorrect: more than one '+' in file, second '+' in %d line\n", num_line + 2);
		return NULL;
	}
	context->target.x = num_c;
	context->target.y = num_line;
	return &context->target;
}
