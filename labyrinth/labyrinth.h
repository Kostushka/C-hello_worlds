#include <stdio.h>

struct Command {
	int mode;
	int num;
	char direction;
};

struct Point {
	int x;
	int y;
};

struct Labyrinth {
	int size;
	char **labyrinth;
	struct Point traveler;
	struct Point target;
};

int init_command(FILE *fp, struct Command *);
void *load_command(FILE *fp, struct Labyrinth *);
char *get_row(int fd, int size, int num_line, struct Point *, struct Point *);
char *get_string(int fd, char *s, int n);
void print_lab(struct Labyrinth *);
struct Labyrinth *init_labyrint(int fd, int file_size);
struct Labyrinth *load_labyrinth(struct Labyrinth *, int fd, int file_size);
void destroy_lab(struct Labyrinth *);
struct Point *get_point(struct Point *, struct Point *, int c, int num_line, int num_c);
int move(struct Labyrinth *, int way);
int get_command(FILE *fp, char *);
int word_count(char *);
char **write_args(char *, int count_args);
int print_command(struct Command *, char **);
void destroy_args(char **);
