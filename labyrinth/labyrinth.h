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

typedef int(*command_handler)(struct Labyrinth *, int, char **); 

struct Block {
	char *key;
	command_handler value;
	struct Block *next;
};

struct Hash {
	struct Block **hashtab;
	int size_hashtab;
	int count_struct;
};

struct Hash *hash_create(int size);
unsigned hashfunc(struct Hash *hash, char *key);
command_handler hash_find(struct Hash *hash, char *key);
int hash_add(struct Hash *hash, char *key, command_handler value);

int direction(struct Labyrinth *lab, int count_args, char **args, int direction);
int direction_left(struct Labyrinth *lab, int count_args, char **args);
int direction_right(struct Labyrinth *lab, int count_args, char **args);
int direction_up(struct Labyrinth *lab, int count_args, char **args);
int direction_down(struct Labyrinth *lab, int count_args, char **args);

// int init_command(FILE *fp, struct Command *, struct Hash *, struct Labyrinth *);
int init_command(FILE *fp, struct Hash *, struct Labyrinth *);
void *load_command(FILE *fp, struct Labyrinth *);
char *get_row(int fd, int size, int num_line, struct Point *, struct Point *);
char *get_string(int fd, char *s, int n);
void print_lab(struct Labyrinth *);
struct Labyrinth *init_labyrint(int fd, int file_size);
struct Labyrinth *load_labyrinth(struct Labyrinth *, int fd, int file_size);
void destroy_lab(struct Labyrinth *);
struct Point *get_point(struct Point *, struct Point *, int c, int num_line, int num_c);
int move(struct Labyrinth *, int way);
int get_command(FILE *fp, char *, int size);
int word_count(char *);
char **write_args(char *, int count_args);
int print_command(struct Command *, char **);
void destroy_args(char **, int count_args);
