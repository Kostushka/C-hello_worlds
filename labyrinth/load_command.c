#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "labyrinth.h"
#define BUFSIZE 64

#define UP                  1   //0001 y ^ -
#define DOWN                2   //0010 y v +
#define LEFT                4   //0100 x < -
#define RIGHT               8   //1000 x > +

#define EACH_STEP          16   //00010000
#define ERROR              32   //00100000
#define CHANGE_DIRECTION   64   //01000000
#define TARGET            128   //10000000

void *load_command(FILE *fp, struct Labyrinth *lab) {

	// создать структуру для команды
	struct Command *command = (struct Command *) malloc(sizeof(struct Command));
	command->mode = -1;
	command->num = -1;
	command->direction = -1;

	int success = 0;

	while (1) {
		char prev_direction = command->direction;
		// функция парсинга строки из файла команд в структуру команды
		int init = init_command(fp, command);
		if (init != 0) {
			if (init == EOF) {
				break;
			}
			free(command);
			return NULL;
		}
		if (prev_direction != -1 && prev_direction != command->direction) {
			if (command->mode == CHANGE_DIRECTION) {
				print_lab(lab);
				printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
				printf("+: {%d; %d}\n", lab->target.x, lab->target.y);		
			}
		}

		// выполнение команды: перемещение по лабиринту
		while (command->num > 0) {
			// движение по лабиринту на один шаг
			if (move(lab, command->direction) != 0) {
				if (command->mode == ERROR) {
					print_lab(lab);
					printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
					printf("+: {%d; %d}\n", lab->target.x, lab->target.y);
				}
				free(command);
				return NULL;
			}
			// отрисовать лабиринт на каждом шаге
			if (command->mode == EACH_STEP) {
				print_lab(lab);
				printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
				printf("+: {%d; %d}\n", lab->target.x, lab->target.y);
			}
			
			if (lab->traveler.x == lab->target.x && lab->traveler.y == lab->target.y) {
				success = 1;
				if (command->mode == TARGET) {
					print_lab(lab);
					printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
					printf("+: {%d; %d}\n", lab->target.x, lab->target.y);
				}
			}
			--command->num;
		}
	}

	if (!success) {
		printf("FAIL!\n");
	} else {
		printf("SUCCESS!\n");
	}

	// очистить память, выделенную под структуру команд
	free(command);
}

int init_command(FILE *fp, struct Command *command) {
	char command_buf[BUFSIZE];
	
	// получаю одну команду из файла команд
	int c = get_command(fp, command_buf, BUFSIZE);
	if (c != 0) {
		if (c == EOF) {
			return EOF;
		} 
		return 1;
	}
	
	// считываю в буфер название команды
	char command_name[BUFSIZE];
	int n = sscanf(command_buf, "%s", command_name);
	if (n != 1) {
		fprintf(stderr, "error read command name\n");
		return 1;
	}

	// посчитать кол-во аргументов команды
	int count_args = word_count(command_buf) - 1;
	if (count_args < 0) {
		fprintf(stderr, "incorrect number of args %d\n", count_args);
		return 1;
	}

	// формирую массив аргументов команды
	char **command_args = write_args(command_buf, count_args);
	if (command_args == NULL) {
		return 1;
	}

	// если считанная строка - команда режима печати
	if (strcmp(command_name, "PRINT_ON") == 0) {
		if (print_command(command, command_args) != 0) {
			destroy_args(command_args, count_args);
			return 1;
		}
		destroy_args(command_args, count_args);
		return 0;
	}

	// если нет аргументов, кол-во выполнений команды по умолчанию: 1
	if (count_args == 0) {
		command->num = 1;
	} else {
		// проверка, что в буфере аргумента число
		if (sscanf(command_args[0], "%d", &command->num) != 1) {
			fprintf(stderr, "arg is not a number\n");
			destroy_args(command_args, count_args);
			return 1;
		}	
	}

	// количество выполнений команды не может быть меньше единицы 
	if (command->num < 1) {
		printf("number of commands executed is less than 1\n");
		destroy_args(command_args, count_args);
		return 1;
	}

	// записываю команду в структуру
	if (strcmp(command_name, "UP") == 0) {
		command->direction = UP;
	} else if (strcmp(command_name, "DOWN") == 0) {
		command->direction = DOWN;
	} else if (strcmp(command_name, "LEFT") == 0) {
		command->direction = LEFT;
	} else if (strcmp(command_name, "RIGHT") == 0){
		command->direction = RIGHT;
	} else {
		fprintf(stderr, "command data not received\n");
		destroy_args(command_args, count_args);
		return 1;
	}
	
	destroy_args(command_args, count_args);
	return 0;	
}

int print_command(struct Command *command, char **command_args) {
	// записываю режим печати в структуру
	if (strcmp(command_args[0], "each_step") == 0) {
		command->mode = EACH_STEP;
	} else if (strcmp(command_args[0], "error") == 0) {
		command->mode = ERROR;
	} else if (strcmp(command_args[0], "change_direction") == 0) {
		command->mode = CHANGE_DIRECTION;
	} else if (strcmp(command_args[0], "target") == 0) {
		command->mode = TARGET;
	} else {
		fprintf(stderr, "error read print mode\n");
		return 1;
	}		
	return 0;
}

int get_command(FILE *fp, char *buf, int size) {
	// считать из файла команд строку до \n или EOF или не больше BUFSIZE (64 байт)
	if (fgets(buf, size, fp) == NULL) {
		if (errno) {
			perror("fgets");
			return 1;
		}
		return EOF;		
	}
	// проверить, что считанная строка корректна: есть EOF
	if (feof(fp)) {
		return 0;
	}
	// проверить, что считанная строка корректна: есть \n
	for (int i = 0;; i++) {
		if (i == size) {
			// \n не было найдено в считанной строке
			fprintf(stderr, "error read command\n");
			return 1;
		}
		if (buf[i] == '\n') {
			return 0;
		}
	}	
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
