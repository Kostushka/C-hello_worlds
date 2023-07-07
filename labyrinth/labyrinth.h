#include <stdio.h>
#include <stdint.h>

struct Point {
	int x;
	int y;
};

struct Labyrinth {
	int size;
	char **labyrinth;;
};

struct Context {
	unsigned int print_mode;
	int move_result;
	int prev_direction;
	int curr_direction;
	struct Point traveler;
	struct Point target;	
};

//typedef int(*command_handler)(struct Context *, struct Labyrinth *, int count_args, char **args); 
typedef int(*command_handler)(struct Context *, struct Labyrinth *, uintptr_t arg, int command_name); 
typedef struct Parse_data *(*parse_handler)(int count_args, char **args);

struct Parse_data {
	// обработчик, который исполняет команду
	command_handler handler;
	// аргумент обработчика
	uintptr_t arg;
};

struct Block {
	char *key;
	void *value;
	struct Block *next;
};

struct Hash {
	struct Block **hashtab;
	int size_hashtab;
	int count_struct;
};

struct Hash *hash_create(int size);
unsigned hashfunc(struct Hash *hash, char *key);
void *hash_find(struct Hash *hash, char *key);
int hash_add(struct Hash *hash, char *key, void *value);
void hash_destroy(struct Hash *hash);

int direction_left(struct Context *, struct Labyrinth *lab, int count_args, char **args);
int direction_right(struct Context *, struct Labyrinth *lab, int count_args, char **args);
int direction_up(struct Context *, struct Labyrinth *lab, int count_args, char **args);
int direction_down(struct Context *, struct Labyrinth *lab, int count_args, char **args);
//int print_on(struct Context *, struct Labyrinth *lab, int	count_args, char **args);

struct Parse_data *parse_print_on(int count_args, char **args);
struct Parse_data *parse_direction(int count_args, char **args);
int direction(struct Context *, struct Labyrinth *lab, uintptr_t arg, int command_name);
int print_on(struct Context *, struct Labyrinth *lab, uintptr_t arg, int command_name);

struct Context *context_init(void);
struct Hash *handlers_init(void);
int handl_command(FILE *fp, struct Hash *, struct Context *, struct Labyrinth *);
int handling_commands(FILE *fp, struct Context *, struct Labyrinth *, struct Hash *);
char *get_row(int fd, int size, int num_line, struct Context *);
char *get_string(int fd, char *s, int n);
void print_lab(struct Context *, struct Labyrinth *);
struct Labyrinth *init_labyrint(int fd, struct Context *, int file_size);
struct Labyrinth *load_labyrinth(struct Context *, struct Labyrinth *, int fd, int file_size);
void destroy_lab(struct Context *, struct Labyrinth *);
struct Point *get_point(struct Context *context, int c, int num_line, int num_c);
int move(struct Context *, struct Labyrinth *, int way);
int get_command(FILE *fp, char *, int size);
int word_count(char *);
char **write_args(char *, int count_args);
int print_command(char **);
void destroy_args(char **, int count_args);
int is_empty(char *str);
void print(struct Context *, struct Labyrinth *);
