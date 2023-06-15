#include <stdio.h>
#include "labyrinth.h"
#define UP      1       //0001 x ^ -
#define DOWN    2       //0010 x v +
#define LEFT    4       //0100 y < -
#define RIGHT   8       //1000 y > +

int move_point(struct Labyrinth *lab, int new_x, int new_y, int z);

int move(struct Labyrinth *lab, int way) {
	int new_x = lab->point.x;
	int new_y = lab->point.y;
	switch(way) {
		case UP:
			// на одну строку назад
			new_y -= 1;
			
			if (move_point(lab, new_x, new_y, new_y) != 0) {
				return 1;
			}
			
			// сдвигаю координаты * вверх на 1 шаг
			lab->point.y -= 1;
			break;
		case DOWN:
			// на одну строку вперед
			new_y += 1;

			if (move_point(lab, new_x, new_y, new_y) != 0) {
				return 1;
			}
			
			// сдвигаю координаты * вниз на 1 шаг
			lab->point.y += 1;
			break;
		case LEFT:
			// на один символ влево в строке
			new_x -= 1;

			if (move_point(lab, new_x, new_y, new_x) != 0) {
				return 1;
			}			
			
			// сдвигаю координаты * влево на 1 шаг
			lab->point.x -= 1;
			break;
		case RIGHT:
			// на один символ вправо в строке
			new_x += 1;

			if (move_point(lab, new_x, new_y, new_x) != 0) {
				return 1;
			}

			// сдвигаю координаты * вправо на 1 шаг
			lab->point.x += 1;
			break;
	}
	return 0;
}

int move_point(struct Labyrinth *lab, int new_x, int new_y, int z) {	
	// проверка, что не вышли за координатную плоскость
	if (z < 0 || z >= lab->size) {
		fprintf(stderr, "out of plane\n");
		return 1;
	}
	// проверка, что нет препятствий
	char up = lab->labyrinth[new_y][new_x];
	if (up != ' ') {
		fprintf(stderr, "ran into an obstacle\n");
		return 1;
	}
	// сдвигаю * на 1 шаг
	lab->labyrinth[lab->point.y][lab->point.x] = ' ';
	lab->labyrinth[new_y][new_x] = '*';
	
	return 0;
}
