#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

	while (feof(fp) == 0) {

		// функция парсинга строки из файла команд в структуру команды
		if (init_command(fp, command) == NULL) {
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

struct Command *init_command(FILE *fp, struct Command *command) {
	char str[100];
	// распарсить строку из файла команд в структуру для команды
	int n = fscanf(fp, "%s %d\n", str, &command->num);
	
	// если число пропущено, кол-во команд по умолчанию: 1
	if (command->num == 0) {
		command->num = 1;
	}
	if (n != 2 && n != 1) {
		fprintf(stderr, "error read command\n");
		return NULL;
	}

	// проверка, записаны ли данные команды в строку
	if (str[0] == 0) {
		fprintf(stderr, "command data not received\n");
		return NULL;
	}

	// записываю команду в структуру
	if (strcmp(str, "UP") == 0) {
		command->flag = UP;
	} else if (strcmp(str, "DOWN") == 0) {
		command->flag = DOWN;
	} else if (strcmp(str, "LEFT") == 0) {
		command->flag = LEFT;
	} else if (strcmp(str, "RIGHT") == 0){
		command->flag = RIGHT;
	}

	// проверка, что команды записаны в структуру
	if (command->flag == -1 || command->num == -1) {
		fprintf(stderr, "command data not received\n");
		return NULL;
	}

	return command;
}
