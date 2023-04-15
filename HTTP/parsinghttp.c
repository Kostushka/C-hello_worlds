#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "parsinghttp.h"
#define SIZE 10

// // структура элемента хэша
// struct Block {
	// char *key;
	// char *value;
	// struct Block *p;
// };
// 
// // структура хэша
// struct Hash {
	// struct Block **hashtab;
	// int size_hashtab;
// };

// структура запроса
struct Http_request {
	char *method;
	char *resource;
	char *protocol_version;
	struct Hash *headers;	
};

void readreq(int fd, struct Http_request *http);

char *read_line(int fd);

void parsing_req(char *s, struct Http_request *http);
void parsing_head(char *s, struct Http_request *http);

int main(int argc, char *argv[]) {

	struct Http_request *http;
	http = (struct Http_request *) malloc(sizeof(struct Http_request));
	if (http == NULL) {
		perror("malloc");
		return 1;
	}

	if (argc == 2) {
		// открыть файл
		int fd = open(argv[1], O_RDONLY, 0);
		// прочитать файл
		readreq(fd, http);
		// закрыть файл
		close(fd);
	}

	return 0;
}

void readreq(int fd, struct Http_request *http) {

	// получаем строку запроса в преобразованном виде
	char *reqline = read_line(fd);
	if (reqline == NULL) {
		return;
	}

	parsing_req(reqline, http);

	// создаем хэш для строк заголовков
	http->headers = hash_create(SIZE);
	if (http->headers == NULL) {
		return;
	}

	struct Hash *hash = hash_create(10);
	hash_add(hash, "cat", "Vasy");
	printf("Name: %s\n", hash_get(hash, "cat"));
	hash_add(hash, "cat", "Georg");
	printf("Name2: %s\n", hash_get(hash, "cat"));
	for (int i = 0; i < 10; i++) {
		if (hash->hashtab[i] == NULL) {
			continue;
		}
		struct Block *p = hash->hashtab[i];
		while (p != NULL) {
			printf("%s %s\n", p->key, p->value);
			p = p->p;
		}
	}
	

	// строка заголовка
	char *headline;

	do {
		// получаем строку заголовка
		headline = read_line(fd);
		if (headline == NULL) {
			return;
		}

		// если пустая строка - выходим из цикла
		if (headline[0] == '\0') {
			break;
		}

		parsing_head(headline, http);
				
	} while (1);

	// выводим содержимое хэша
	for (int i = 0; i < http->headers->size_hashtab; i++) {
		if (http->headers->hashtab[i] == NULL) {
			continue;
		}
		struct Block *p = http->headers->hashtab[i];
		while (p != NULL) {
			printf("%s %s\n", p->key, p->value);
			p = p->p;
		}
	}
}

char *read_line(int fd) {

	// буфер для посимвольного чтения файла 
	char c;
	
	int size, count;
	size = 128;
	count = 0;

	// выделяем память под строку файла
	char *strp;
	strp = (char *) calloc(size, sizeof(char));
	if (strp == NULL) {
		perror("calloc");
		return NULL;
	}

	do {
		// если выделенная память заполнена, расширяем ее размер
		if (count == size) {
			size *= 2;
			strp = (char *) realloc(strp, size * sizeof(char));
			if (strp == NULL) {
				perror("realloc");
				return NULL;
			}
		}

		// читаем один символ файла
		if (read(fd, &c, 1) == -1) {
			perror("read");
			return NULL;
		}

		if (c != '\n') {
			// добавляем в выделенную под строку память значение каждого не \n символа
			strp[count++] = c;
		} else {
			// заменяем \n на символ конца строки
			strp[count++] = '\0';
		}
		
	} while (c != '\n');
		
	return strp;
}

void parsing_req(char *s, struct Http_request *http) {

	int len = strlen(s);
	
	// проходимся по всей строке и заменяем ' ' на символ конца строки - для последующего извлечения из строки слов
	// GET / HTTP/1.1\n -> GET\0/\0HTTP/1.1\0
	for (int i = 0; i < len; i++) {		
		if (s[i] == ' ') {	
			s[i] = '\0';
		}
	}

	// индекс для передачи адреса начала нужного слова
	int start = 0;

	// записываем метод
	http->method = &s[start];
		
	while (s[start] != '\0') {
		++start;
	}

	// записываем url
	http->resource = &s[++start];

	while (s[start] != '\0') {
		++start;
	}

	// записываем версию протокола
	http->protocol_version = &s[++start];

	printf("method: %s\nresource: %s\nver: %s\n", http->method, http->resource, http->protocol_version);
	
}

void parsing_head(char *s, struct Http_request *http) {
	for (int i = 0; i < (int) strlen(s); i++) {	
		// если встречаем комбинацию ': ', то заменяем ее на ':\0' - это разделитель между key и value заголовка
		// connection: close -> connection:\0close
		if (s[i] == ':' && s[i + 1] == ' ') {				
			s[i + 1] = '\0';
			break;
		}
	}

	int start = 0;

	// получаем строку ключа
	char *key = &s[start];
	
	while (s[start] != '\0') {
		++start;
	}

	// получаем строку значения
	char *value = &s[++start];

	// добавляем строку заголовка в хэш
	if (hash_add(http->headers, key, value) == NULL) {
		return;
	}
}
