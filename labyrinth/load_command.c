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

	// 64 - \n - \0 = 62 байта строки команды
	char command_buf[61]; // 60 байт + \0
	char num_buf[3]; // 2 байта + \0

	int i, k;
	for (i = 0; i < 64; i++) {
		// если буфер переполнен: ошибка
		if (sizeof(command_buf) == i) {
			fprintf(stderr, "error read command\n");
			return 1;
		}
		if (str[i] != ' ') {
			command_buf[i] = str[i];
		} else {
			command_buf[i] = '\0';
			break;
		}
	}

	// пропустили пробел
	++i;
	
	for (k = 0; i < 64; i++, k++) {
		// если буфер переполнен: ошибка
		if (sizeof(num_buf) == k) {
			fprintf(stderr, "error read command\n");
			return 1;
		}
		if (str[i] != '\n' && str[i] != ' ') {
			num_buf[k] = str[i]; 
		} else {
			num_buf[k] = '\0';
			break;
		}
	}

	// если буфер с числом непустой, распарсить в структуру для команды
	if (k > 0 && sscanf(num_buf, "%d", &command->num) != 1) {
		fprintf(stderr, "error read command\n");
		return 1;
	}

	// если буфер с числом пуст, кол-во команд по умолчанию: 1
	if (k == 0) {
		command->num = 1;
	}

	// проверка, что число команды положительно
	if (command->num < 0) {
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
	}

	// проверка, что команды записаны в структуру
	if (command->flag == -1) {
		fprintf(stderr, "command data not received\n");
		return 1;
	}

	return 0;
}
