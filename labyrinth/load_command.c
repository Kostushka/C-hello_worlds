#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "labyrinth.h"
#define BUFSIZE 64

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
	char num_buf[BUFSIZE];

	// считываю строку в буфер под название команды и в буфер под число
	int n = sscanf(str, "%s %s", command_buf, num_buf);

	switch(n) {
		// если буфер с числом не пуст
		case 2: 
			// проверка, что в буфере число
			if (sscanf(num_buf, "%d", &command->num) != 1) {
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
		command->flag = UP;
	} else if (strcmp(command_buf, "DOWN") == 0) {
		command->flag = DOWN;
	} else if (strcmp(command_buf, "LEFT") == 0) {
		command->flag = LEFT;
	} else if (strcmp(command_buf, "RIGHT") == 0){
		command->flag = RIGHT;
	} else {
		fprintf(stderr, "command data not received\n");
		return 1;
	}
	
	return 0;	
}
