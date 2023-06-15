#include <stdio.h>
#include "labyrinth.h"
#define UP      1       //0001 x ^ -
#define DOWN    2       //0010 x v +
#define LEFT    4       //0100 y < -
#define RIGHT   8       //1000 y > +

int move(struct Labyrinth *lab, int way) {
	int x = lab->point.x;
	int y = lab->point.y;
	switch(way) {
		case UP:
			// проверка, движения на одну строку назад
			x -= 1;
			// проверка, что не вышли за координатную плоскость
			if (x < 0 || x >= lab->size) {
			fprintf(stderr, "out of plane\n");
			return 1;
			}
			// проверка, что нет препятствий
			char up = lab->labyrinth[x][y];
			if (up != ' ') {
				fprintf(stderr, "ran into an obstacle\n");
				return 1;
			}
			// сдвигаю * вверх на 1 шаг
			lab->point.x -= 1;
			lab->labyrinth[x][y] = '*';
			lab->labyrinth[x + 1][y] = ' ';
			break;
		case DOWN:
			// проверка, движения на одну строку вперед
			x += 1;
			// проверка, что не вышли за координатную плоскость
			if (x < 0 || x >= lab->size) {
			fprintf(stderr, "out of plane\n");
			return 1;
			}
			// проверка, что нет препятствий
			char down = lab->labyrinth[x][y];
			if (down != ' ') {
			fprintf(stderr, "ran into an obstacle\n");
			return 1;
			}
			// сдвигаю * вниз на 1 шаг
			lab->point.x += 1;
			lab->labyrinth[x][y] = '*';
			lab->labyrinth[x - 1][y] = ' ';
			break;
		case LEFT:
			// проверка, движения в строке на один символ влево
			y -= 1;
			// проверка, что не вышли за координатную плоскость
			if (y < 0 || y >= lab->size) {
			fprintf(stderr, "out of plane\n");
			return 1;
			}
			// проверка, что нет препятствий
			char left = lab->labyrinth[x][y];
			if (left != ' ') {
			fprintf(stderr, "ran into an obstacle\n");
			return 1;
			}
			// сдвигаю * влево на 1 шаг
			lab->point.y -= 1;
			lab->labyrinth[x][y] = '*';
			lab->labyrinth[x][y + 1] = ' ';
			break;
		case RIGHT:
			// проверка, движения в строке на один символ вправо
			y += 1;
			// проверка, что не вышли за координатную плоскость
			if (y < 0 || y > lab->size) {
			fprintf(stderr, "out of plane\n");
			return 1;
			}
			// проверка, что нет препятствий
			char right = lab->labyrinth[x][y];
			if (right != ' ') {
			fprintf(stderr, "ran into an obstacle\n");
			return 1;
			}
			// сдвигаю * вправо на 1 шаг
			lab->point.y += 1;
			lab->labyrinth[x][y] = '*';
			lab->labyrinth[x][y - 1] = ' ';
			break;
	}
	return 0;
}

