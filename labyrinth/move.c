#include <stdio.h>
#include "labyrinth.h"
#define UP      1       //0001 y ^ -
#define DOWN    2       //0010 y v +
#define LEFT    4       //0100 x < -
#define RIGHT   8       //1000 x > +

int move(struct Context *context, struct Labyrinth *lab, int way) {
	int new_x = context->traveler.x;
	int new_y = context->traveler.y;
	switch(way) {
		case UP:
			// на один указатель назад в массиве указателей 
			new_y -= 1;
			break;
		case DOWN:
			// на один указатель вперед в массиве указателей
			new_y += 1;
			break;
		case LEFT:
			// на один символ влево в массиве байт
			new_x -= 1;
			break;
		case RIGHT:
			// на один символ вправо в массиве байт
			new_x += 1;
			break;
	}
	
	// проверка, что не вышли за координатную плоскость
	if (new_x < 0 || new_x >= lab->size) {
		fprintf(stderr, "out of plane\n");
		return 1;
	}
	if (new_y < 0 || new_y >= lab->size) {
		fprintf(stderr, "out of plane\n");
		return 1;
	}
	
	// проверка, что нет препятствий
	char c = lab->labyrinth[new_y][new_x];
	if (c != ' ' && c != '+') {
		fprintf(stderr, "ran into an obstacle\n");
		return 1;
	}

	// сдвигаю * на 1 шаг
	lab->labyrinth[context->traveler.y][context->traveler.x] = ' ';
	context->traveler.y = new_y;
	context->traveler.x = new_x;
	lab->labyrinth[new_y][new_x] = '*';
	
	return 0;
}

