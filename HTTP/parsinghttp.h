// структура элемента хэша
struct Block {
	char *key;
	char *value;
	struct Block *next;
};

// структура хэша
struct Hash {
	struct Block **hashtab;
	int size_hashtab;
};

struct Hash *hash_create(int size);
unsigned hash_func(struct Hash *hash, char *key);
struct Block *hash_find(struct Hash *hash, char *key);
char *hash_add(struct Hash *hash, char *key, char *value);
char *hash_get(struct Hash *hash, char *key);
char **hash_keys(struct Hash *hash);
void hash_delete(struct Hash *hash);
