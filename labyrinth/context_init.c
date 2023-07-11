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
	context->traveler_step_up.x = -1;
	context->traveler_step_up.y = -1;
	context->traveler_step_down.x = -1;
	context->traveler_step_down.y = -1;
	context->traveler_step_left.x = -1;
	context->traveler_step_left.y = -1;
	context->traveler_step_right.x = -1;
	context->traveler_step_right.y = -1;
	context->target.x = -1;
	context->target.y = -1;
	
	return context;
}

void write_step_forward(struct Context *context) {
	context->traveler_step_up.x = context->traveler.x;
	context->traveler_step_up.y = context->traveler.y + 1;

	context->traveler_step_down.x = context->traveler.x;
	context->traveler_step_down.y = context->traveler.y - 1;

	context->traveler_step_left.x = context->traveler.x - 1;
	context->traveler_step_left.y = context->traveler.y;

	context->traveler_step_right.x = context->traveler.x + 1;
	context->traveler_step_right.y = context->traveler.y;
}
