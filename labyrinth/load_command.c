#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	command->print = "PRINT_ON"; 
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
			if (move(lab, command->direction) != 0) {
				if (command->mode == ERROR) {
					print_lab(lab);
					printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
					printf("+: {%d; %d}\n", lab->target.x, lab->target.y);
				}
				free(command);
				return NULL;
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

		// отрисовать лабиринт на каждом шаге
		if (command->mode == EACH_STEP) {
			print_lab(lab);
			printf("*: {%d; %d}\n", lab->traveler.x, lab->traveler.y);
			printf("+: {%d; %d}\n", lab->target.x, lab->target.y);
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
	char str[BUFSIZE];
	// считать из файла команд строку до \n или не больше BUFSIZE (64 байт)
	if (fgets(str, sizeof(str), fp) == NULL) {
		if (errno) {
			perror("fgets");
			return 1;
		}
		return EOF;		
	}

	// проверить, что считанная строка корректна: есть \n
	for (int i = 0;; i++) {
		if (i == sizeof(str)) {
			// \n не было найдено в считанной строке
			fprintf(stderr, "error read command\n");
			return 1;
		}
		if (str[i] == '\n') {
			break;
		}
	}

	char command_buf[BUFSIZE];
	char arg_buf[BUFSIZE];

	// считываю строку в буфер под название команды и в буфер под аргумент команды
	int n = sscanf(str, "%s %s", command_buf, arg_buf);

	// если считанная строка - команда режима печати
	if (strcmp(command_buf, command->print) == 0) {
		// второй буфер не должен быть пустым
		if (n != 2) {
			fprintf(stderr, "error read command\n");
			return 1;
		}

		// записываю режим печати в структуру
		if (strcmp(arg_buf, "each_step") == 0) {
			command->mode = EACH_STEP;
		} else if (strcmp(arg_buf, "error") == 0) {
			command->mode = ERROR;
		} else if (strcmp(arg_buf, "change_direction") == 0) {
			command->mode = CHANGE_DIRECTION;
		} else if (strcmp(arg_buf, "target") == 0) {
			command->mode = TARGET;
		} else {
			fprintf(stderr, "error read print mode\n");
			return 1;
		}		
		return 0;
	}

	switch(n) {
		// если буфер с числом не пуст
		case 2: 
			// проверка, что в буфере число
			if (sscanf(arg_buf, "%d", &command->num) != 1) {
				fprintf(stderr, "error read command\n");
				return 1;
			}
			break;
		case 1:
		 	// кол-во команд по умолчанию: 1
		 	command->num = 1;
		 	break;
		 default:
		 	fprintf(stderr, "error read command\n");
			return 1;
	}

	// количество выполнений команды не может быть меньше единицы 
	if (command->num < 1) {
		printf("error read command\n");
		return 1;
	}

	// записываю команду в структуру
	if (strcmp(command_buf, "UP") == 0) {
		command->direction = UP;
	} else if (strcmp(command_buf, "DOWN") == 0) {
		command->direction = DOWN;
	} else if (strcmp(command_buf, "LEFT") == 0) {
		command->direction = LEFT;
	} else if (strcmp(command_buf, "RIGHT") == 0){
		command->direction = RIGHT;
	} else {
		fprintf(stderr, "command data not received\n");
		return 1;
	}
	
	return 0;	
}
