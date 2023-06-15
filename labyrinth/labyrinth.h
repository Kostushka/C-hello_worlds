struct Point {
	int x;
	int y;
};

struct Labyrinth {
	int size;
	char **labyrinth;
	struct Point point;
};

struct Labyrinth *init_labyrint(int fd, int file_size);
char *get_row(int fd, int size, int num_line, struct Point *);
char *get_string(int fd, char *s, int n);
void print_lab(struct Labyrinth *);
struct Labyrinth *load_labyrinth(struct Labyrinth *, int fd, int file_size);
void destroy_lab(struct Labyrinth *);
struct Point *get_point(struct Point *, int c, int num_line, int num_c);
int move(struct Labyrinth *, int way);
