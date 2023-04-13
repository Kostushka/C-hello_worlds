#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

// структура заголовков
struct Headers {
	char *host;
	char *connection;
	char *user_agent;
	char *date;
	char *lg;	
};

// структура запроса
struct Http_request {
	char *method;
	char *url;
	char *protocol_version;
	struct Headers *headers;	
};

void readreq(int fd);

int main(int argc, char *argv[]) {

	if (argc == 2) {
		// открыть файл
		int fd = open(argv[1], O_RDONLY, 0);
		// прочитать файл
		readreq(fd);
		// закрыть файл
		close(fd);
	}

	return 0;
}

void readreq(int fd) {

	// буфер для посимвольного чтения файла 
	char buf[1];
	int n, size, count;
	size = 5;
	count = 0;

	// выделяем память под строку файла
	char *strp;
	strp = (char *) calloc(size, sizeof(char));
	if (strp == NULL) {
		perror("calloc");
		return;
	}

	// пока не конец файла
	while ((n = read(fd, buf, 1)) != 0) {

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
		if (*buf != '\n') {
			// инкрементируем счетчик записанных в память байт
			++count;
			// добавляем в выделенную под строку память копию каждого не \n символа
			strcat(strp, strdup(buf));
		} else {
			printf("%s\n", strp);
		}
	}

}
