#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

// структура заголовков
struct Headers {
	char *key;
	char *value;
};

// структура запроса
struct Http_request {
	char *method;
	char *url;
	char *protocol_version;
	struct Headers **headers;	
};

void readreq(int fd, struct Http_request *http);

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

	// буфер для посимвольного чтения файла 
	char c;
	int n, size, count, i;
	size = 5;
	count = i = 0;

	// выделяем память под строку файла
	char *strp;
	strp = (char *) calloc(size, sizeof(char));
	if (strp == NULL) {
		perror("calloc");
		return;
	}

	// пока не конец файла
	while ((n = read(fd, &c, 1)) != 0) {

		// если выделенная память заполнена, расширяем ее размер
		if (count == size) {
			size *= 2;
			strp = (char *) realloc(strp, size * sizeof(char));
			if (strp == NULL) {
				perror("realloc");
				return;
			}
		}

		// если символ не \n
		if (c != '\n') {
			// добавляем в выделенную под строку память значение каждого не \n символа
			strp[count++] = c;
		} else {
			strp[count++] = '\0';
			for (; i < count; i++) {
				if (strp[i] == ' ') {
					strp[i] = '\0';
				} 
			}	
		}
	}

	int start = 0;
	http->method = &strp[start];
	
	while (strp[start] != '\0') {
		++start;
	}
	http->url = &strp[++start];
	while (strp[start] != '\0') {
		++start;
	}
	http->protocol_version = &strp[++start];
		++start;

	printf("method: %s\nurl: %s\nver: %s\n", http->method, http->url, http->protocol_version);

}
