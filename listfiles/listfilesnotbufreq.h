struct Block {
	char *key;
	int value;
	struct Block *p;
};

struct Hash {
	struct Block **hashtab;
	int count_struct;
	int size_hashtab;	
	int count_fill_el;
};
struct Hash *create_hash(int);
unsigned hashfunc(struct Hash *, char *);
int find_hash(struct Hash *, char *);
int add_hash(struct Hash *, char *);

int get_not_duplpaths(char **, int, char ***);

