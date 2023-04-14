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

struct Hash *hash_create(int size);
unsigned hash_func(struct Hash *hash, char *key);
char *hash_find(struct Hash *hash, char *key);
char *hash_add(struct Hash *hash, char *key, char *value);
