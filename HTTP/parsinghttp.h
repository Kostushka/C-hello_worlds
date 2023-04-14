// структура элемента хэша
struct Block {
	char *key;
	char *value;
	struct Block *p;
};

// структура хэша
struct Hash {
	struct Block **hashtab;
	int size_hashtab;
};

struct Hash *create_hash(int size);
unsigned hashfunc(struct Hash *hash, char *key);
struct Block *find_hash(struct Hash *hash, char *key);
char *add_hash(struct Hash *hash, char *key, char *value);
