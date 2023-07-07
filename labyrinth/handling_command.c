#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "labyrinth.h"
#define BUFSIZE 64

#define SUCCESS_HANDLING_COMMAND 0
#define IS_COMMENT 1
#define ERR_GET_COMMAND 2
#define ERR_FATAL -2
#define ERR_HANDLING_COMMAND 1

#define UP                  1   //0001 y ^ -
#define DOWN                2   //0010 y v +
#define LEFT                4   //0100 x < -
#define RIGHT               8   //1000 x > +

#define EACH_STEP          16   //00010000
#define ERROR              32   //00100000
#define CHANGE_DIRECTION   64   //01000000
#define TARGET            128   //10000000

int handling_commands(FILE *fp, struct Context *context, struct Labyrinth *lab, struct Hash *command_data) {

	// отрисовать лабиринт
	printf("-НАЧАЛО-\n");
	print_lab(context, lab);

	while (1) {
		// функция парсинга строки из файла команд и выполнения команды
		int command = handle_command(fp, command_data, context, lab);
		if (command != SUCCESS_HANDLING_COMMAND && command != IS_COMMENT) {
			if (command == EOF) {
				break;
			}
			return ERR_HANDLING_COMMAND;
		}
	}
	return 0;
}

int handle_command(FILE *fp, struct Hash *command_data, struct Context *context, struct Labyrinth *lab) {

	char command_buf[BUFSIZE];
	// получаю одну команду из файла команд
	int c = get_command(fp, command_buf, BUFSIZE);
	switch(c) {
		// комманда успешно получена
		case 0:
			break;
		// комментарий пропускаем
		case IS_COMMENT:
			return IS_COMMENT;
		case EOF:
			return EOF;
		case ERR_GET_COMMAND:
			return ERR_GET_COMMAND;
		default:
			fprintf(stderr, "fatal error: get_command return %d\n", c);
			return ERR_FATAL;
	}
	
	// считываю в буфер название команды
	char command_name[BUFSIZE];
	int n = sscanf(command_buf, "%s", command_name);
	if (n != 1) {
		fprintf(stderr, "error read command name\n");
		return -1;
	}

	// посчитать кол-во аргументов команды
	int count_args = word_count(command_buf) - 1;
	if (count_args < 0) {
		fprintf(stderr, "incorrect number of args %d\n", count_args);
		return -1;
	}

	// формирую массив аргументов команды
	char **command_args = write_args(command_buf, count_args);
	if (command_args == NULL) {
		return -1;
	}

	// получаю обработчик команды по имени
	parse_handler parse = hash_find(command_data, command_name);
	if (parse == NULL) {
		fprintf(stderr, "handler for command %s not found in hash\n", command_name);
		destroy_args(command_args, count_args);
		return -1;
	}
	
	// вызываю выполнение обработчика команды
	struct Parse_data *handler;
	if ((handler = parse(count_args, command_args)) == NULL) {
		destroy_args(command_args, count_args);
		return -1;
	}
	int name;
	if (strcmp(command_name, "UP") == 0) {
		name = UP;
	} else if (strcmp(command_name, "DOWN") == 0) {
		name = DOWN;
	} else if (strcmp(command_name, "LEFT") == 0) {
		name = LEFT;
	} else if (strcmp(command_name, "RIGHT") == 0) {
		name = RIGHT;
	}
	// вызываю выполнение обработчика, исполняющего команду
	handler->handler(context, lab, handler->arg, name);
	
	free(handler);
	destroy_args(command_args, count_args);
	
	return 0;
}

int get_command(FILE *fp, char *buf, int size) {
	// считать из файла команд строку до \n или EOF или не больше BUFSIZE (64 байт)
	if (fgets(buf, size, fp) == NULL) {
		if (errno) {
			perror("fgets");
			return ERR_GET_COMMAND;
		}
		return EOF;		
	}
	// проверить, что считанная строка корректна: есть EOF
	if (feof(fp)) {
		// проверка на комментарий
		if (is_empty(buf)) {
			return IS_COMMENT;
		}
		return 0;
	}
	// проверить, что считанная строка корректна: есть \n
	for (int i = 0;; i++) {
		if (i == size) {
			// \n не было найдено в считанной строке
			fprintf(stderr, "error read command %s\n", buf);
			return ERR_GET_COMMAND;
		}
		if (buf[i] == '\n') {
			// проверка на комментарий
			if (is_empty(buf)) {
				return IS_COMMENT;
			}
			return 0;
		}
	}
	return ERR_FATAL;		
}
// # __# ___
int is_empty(char *str) {
	int i;
	// пропуск пробельных символов
	for (i = 0; isspace(str[i]); i++);
	// команда не должна начинаться с # или состоять из пробельных символов
	if (str[i] == '#' || str[i] == '\0') {
		return 1;
	}
	while (str[i] != '\n' && str[i] != '\0') {
		// исключаем комментарий из строки команды
		if (str[i] == '#') {
			str[i] = '\0';
			return 0;
		}
		++i;
	}
	return 0;
}

int word_count(char *buf) {
	int count, state, in, out;
	count = state = out = 0;
	in = 1;
	
	while (*buf != '\0') {
		// инкрементируем счетчик, если пробельный символ идет после слова
		if (*buf == ' ' || *buf == '\t' || *buf == '\n') {
			if (state == in) {
				++count;
				state = out;
			}
		} else {
			state = in;
		}	
		
		++buf;
	}
	if (state == in) {
		++count;
	}
	return count;
}

char **write_args(char *buf, int count_args) {
	// создать массив аргументов
	char **args = (char **) calloc(count_args, sizeof(char *));
	if (args == NULL) {
		perror("calloc");
		return NULL;
	}
	
	int i, j, k;
	// пропускаем имя команды
	for (j = 0; !isspace(buf[j]); j++) {
		if (buf[j] == '\0') {
			return args;
		}
	}
	for (; isspace(buf[j]); j++);
	
	// нет аргументов
	if (buf[j] == '\0') {
		return args;
	}
	
	// записать в массив аргументы
	for (i = 0; i < count_args; i++) {
		// формируем строку аргумента
		char *s = (char *) malloc(sizeof(char) * BUFSIZE);
		if (s == NULL) {
			perror("malloc");
			return NULL;
		}
		for (k = 0; buf[j] != ' ' && buf[j] != '\t' && buf[j] != '\n'; j++, k++) {
			s[k] = buf[j];
		}
		// пропуск пробельного символа
		++j;
		
		s[k] = '\0';

		// записываем строку в массив аргументов
		args[i] = s;
	}

	return args;
}

void destroy_args(char **command_args, int count_args) {
	for (int i = 0; i < count_args && command_args[i] != NULL; i++) {
		free(command_args[i]);
	}
	free(command_args);
}
