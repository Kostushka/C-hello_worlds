struct Block {
	char *key;
	int value;
	struct Block *p;
};

unsigned hashfunc(char *);
int findhash(char *, struct Block **);
int addhash(struct Block **, char *);
