struct Block {
	char *key;
	int value;
	struct Block *p;
};

struct Hash {
	struct Block **hashtab;
	int count_struct;
	int size_hashtab;	
};

unsigned hashfunc(char *);
int findhash(char *, struct Block **);
int addhash(char *, struct Hash **);

int getNotDuplPaths(char **, int, char ***);

