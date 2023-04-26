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

// структура http
struct Http_request {
	char *method;
	char *resource;
	char *protocol_version;
	struct Hash *headers;
};

struct Hash *hash_create(int size);
unsigned hash_func(struct Hash *hash, char *key);
struct Block *hash_find(struct Hash *hash, char *key);
char *hash_add(struct Hash *hash, char *key, char *value);
char *hash_get(struct Hash *hash, char *key);
char **hash_keys(struct Hash *hash);
void hash_delete(struct Hash *hash);

void readreq(int fd, struct Http_request *http);

void writeres(char *filename, int out_fd);
