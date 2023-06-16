#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "labyrinth.h"

#define UP      1       //0001 y ^ -
#define DOWN    2       //0010 y v +
#define LEFT    4       //0100 x < -
#define RIGHT   8       //1000 x > +

void *load_command(FILE *fp, struct Labyrinth *lab) {

	// создать структуру для команды
	struct Command *command = (struct Command *) malloc(sizeof(struct Command));  
	command->num = -1;
	command->flag = -1;

	int success = 0;

	while (1) {

		// функция парсинга строки из файла команд в структуру команды
		int init = init_command(fp, command);
		if (init != 0) {
			if (init == EOF) {
				break;
			}
			free(command);
			return NULL;
		}

		// выполнение команды: перемещение по лабиринту
		while (command->num > 0) {
			if (move(lab, command->flag) != 0) {
				free(command);
				return NULL;
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
	char str[64];
	// считать из файла команд строку до \n или не больше 64 байт
	if (fgets(str, sizeof(str), fp) == NULL) {
		if (errno) {
			perror("fgets");
			return 1;
		}
		return EOF;		
	}
	// проверить, что считанная строка корректна: есть \n
	char flag = 0;
	for (unsigned int i = 0; i < sizeof(str); i++) {
		if (str[i] == '\n') {
			flag = 1;
			break;
		}
	}
	if (!flag) {
		fprintf(stderr, "error read command\n");
		return 1;
	}
	char buf[64];
	
	// распарсить строку в структуру для команды
	int n = sscanf(str, "%s %d", buf, &command->num);

	// если число пропущено, кол-во команд по умолчанию: 1
	if (command->num == 0) {
		// проверка на корректность команды
		char error_str[64];
		if (sscanf(str, "%s %s", error_str, error_str) == 2) {
			printf("error read command: %s\n", error_str);
			return 1;
		}
		command->num = 1;
	}
	
	if (n != 2 && n != 1) {
		fprintf(stderr, "error read command\n");
		return 1;
	}

	// проверка, записаны ли данные команды в строку
	if (buf[0] == 0) {
		fprintf(stderr, "command data not received\n");
		return 1;
	}

	// записываю команду в структуру
	if (strcmp(buf, "UP") == 0) {
		command->flag = UP;
	} else if (strcmp(buf, "DOWN") == 0) {
		command->flag = DOWN;
	} else if (strcmp(buf, "LEFT") == 0) {
		command->flag = LEFT;
	} else if (strcmp(buf, "RIGHT") == 0){
		command->flag = RIGHT;
	}

	// проверка, что команды записаны в структуру
	if (command->flag == -1 || command->num == -1) {
		fprintf(stderr, "command data not received\n");
		return 1;
	}

	return 0;
}
