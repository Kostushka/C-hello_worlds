#include <stdio.h>
#include <stdlib.h>
#include "labyrinth.h"

struct Context *context_init(void) {
	struct Context *context = (struct Context *) malloc(sizeof(struct Context));
	if (context == NULL) {
		perror("malloc");
		return NULL;
	}
	context->print_mode = 0;
	context->move_result = -1;
	context->prev_direction = -1;
	context->curr_direction = -1;
	context->traveler.x = -1;
	context->traveler.y = -1;
	context->target.x = -1;
	context->target.y = -1;
	
	return context;
}
