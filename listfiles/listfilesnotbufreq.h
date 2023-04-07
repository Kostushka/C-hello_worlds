struct Block {
	char *key;
	int value;
	struct Block *p;
};

struct Hash {
	struct Block **hashtab;
	int count_struct;
	int size_hashtab;	
	int count_fillel;
};
struct Hash *createhash(int);
unsigned hashfunc(struct Hash *, char *);
int findhash(struct Hash *, char *);
int addhash(struct Hash *, char *);

int getNotDuplPaths(char **, int, char ***);

