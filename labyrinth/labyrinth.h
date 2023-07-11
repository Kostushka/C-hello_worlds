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
	struct Point traveler_step_up;
	struct Point traveler_step_down;
	struct Point traveler_step_left;
	struct Point traveler_step_right;
	struct Point target;	
};

typedef int(*command_handler)(struct Context *, struct Labyrinth *, void *arg); 
typedef struct Parse_data *(*parse_handler)(struct Context *, int count_args, char *command_name, char **args);

struct Parse_data {
	// обработчик, который исполняет команду
	command_handler handler;
	// аргумент обработчика
	void *arg;
};

struct Move_arg {
	int num;
	int is_to_obstacle;
	int direction;	
};

typedef struct Move_arg move_args_t;

struct Print_arg {
	unsigned int mode;
};

typedef struct Print_arg print_args_t;

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

struct Parse_data *parse_print_on(struct Context *, int count_args, char *command_name, char **args);
struct Parse_data *parse_move(struct Context *, int count_args, char *command_name, char **args);
int move(struct Context *, struct Labyrinth *lab, void *arg);
int print_on(struct Context *, struct Labyrinth *lab, void *arg);

struct Context *context_init(void);
struct Hash *handlers_init(void);
int handle_command(FILE *fp, struct Hash *, struct Context *, struct Labyrinth *);
int handling_commands(FILE *fp, struct Context *, struct Labyrinth *, struct Hash *);
char *get_row(int fd, int size, int num_line, struct Context *);
char *get_string(int fd, char *s, int n);
void print_lab(struct Context *, struct Labyrinth *);
struct Labyrinth *init_labyrint(int fd, struct Context *, int file_size);
struct Labyrinth *load_labyrinth(struct Context *, struct Labyrinth *, int fd, int file_size);
void destroy_lab(struct Context *, struct Labyrinth *);
struct Point *get_point(struct Context *context, int c, int num_line, int num_c);
int step(struct Context *, struct Labyrinth *, int way);
int get_command(FILE *fp, char *, int size);
int word_count(char *);
char **write_args(char *, int count_args);
void destroy_args(char **, int count_args);
int is_empty(char *str);
void print(struct Context *, struct Labyrinth *);
void write_step_forward(struct Context *);
struct Point get_step_forward(struct Context *, int direction, int *, int *);
