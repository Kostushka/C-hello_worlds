struct Block {
	char *key;
	int value;
	struct Block *p;
};
unsigned hashfunc(char *);
int findhash(char *);
int addhash(char *);

int getNotDuplPaths(char **, int, char ***);

