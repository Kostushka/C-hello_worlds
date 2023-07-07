#include <stdio.h>
#include "labyrinth.h"
#define COMMAND_SIZE(x) (int) (sizeof(x) / sizeof(x[0]))

// массив структур с командами и их обработчиками
struct Command {
	char *command;
	parse_handler handler;
} command_data[] = {
	{"UP", parse_move},
	{"DOWN", parse_move},
	{"LEFT", parse_move},
	{"RIGHT", parse_move},
	{"PRINT_ON", parse_print_on},
};

struct Hash *handlers_init(void) {
	// создать хэш
	struct Hash *hash = hash_create(COMMAND_SIZE(command_data));
	if (hash == NULL) {
		return NULL;
	}
	// инициализировать хэш
	for (int i = 0; i < COMMAND_SIZE(command_data); i++) {
		if (hash_add(hash, command_data[i].command, command_data[i].handler) != 0) {
			return NULL;
		}
	}
	return hash;
}
