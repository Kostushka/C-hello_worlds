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

	// индекс для передачи адреса начала нужного слова
	int start = 0;

	// записываем метод
	http->method = &reqline[start];
		
	while (reqline[start] != '\0') {
		++start;
	}

	// записываем url
	http->resource = &reqline[++start];

	
	while (reqline[start] != '\0') {
		++start;
	}

	// записываем версию протокола
	http->protocol_version = &reqline[++start];

	printf("method: %s\nresource: %s\nver: %s\n", http->method, http->resource, http->protocol_version);

	// создаем хэш для строк заголовков
	http->headers = create_hash(SIZE);

	// строка заголовка
	char *headline;

	do {
		int start = 0;

		// получаем строку заголовка
		headline = read_line(fd);

		// если пустая строка - выходим из цикла
		if (headline[0] == '\0') {
			break;
		}

		// получаем строку ключа
		char *key = &headline[start];
		
		while (headline[start] != '\0') {
			++start;
		}

		// получаем строку значения
		char *value = &headline[++start];

		// добавляем строку заголовка в хэш
		add_hash(http->headers, key, value);
				
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
	
	int n, size, count;
	size = 5;
	count = 0;

	// выделяем память под строку файла
	char *strp;
	strp = (char *) calloc(size, sizeof(char));
	if (strp == NULL) {
		perror("calloc");
		exit(1);
	}

	do {
		// если выделенная память заполнена, расширяем ее размер
		if (count == size) {
			size *= 2;
			strp = (char *) realloc(strp, size * sizeof(char));
			if (strp == NULL) {
				perror("realloc");
				exit(1);
			}
		}

		// читаем один символ файла
		n = read(fd, &c, 1);
		if (n == -1) {
			perror("read");
			exit(1);
		}

		if (c != '\n') {
			// добавляем в выделенную под строку память значение каждого не \n символа
			strp[count++] = c;
		} else {
			// заменяем \n на символ конца строки
			strp[count++] = '\0';
			// проходимся по всей строке и заменяем ' ' на символ конца строки - для последующего извлечения из строки слов
			// GET / HTTP/1.1\n -> GET\0/\0HTTP/1.1\0
			for (int i = 0; i < count; i++) {				
				if (strp[i] == ' ') {				
					strp[i] = '\0';
					// если анализируем поле заголовка, то заменять символом конца строки все ' ' не нужно
					if (strp[i - 1] == ':') {
						break;
					}
				}
			}
		}
		
	} while (c != '\n');
		
	return strp;
}
