#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "parsinghttp.h"

void writeres(char *filename, int out_fd) {
	// буфер для содержимого файла
	char buf[BUFSIZ];
	int n;

	// открыть файл
	int fd = open(filename, O_RDONLY, 0);
	if (fd == -1) {
		fprintf(stderr, "%s: %s\n", filename, strerror(errno));
		return;
	}

	// прочитать файл в буфер
	while ((n = read(fd, buf, BUFSIZ)) != 0) {
		if (n == -1) {
			perror("read");
			return;
		}

		// записать буфер в fd сокета
		if (write(out_fd, buf, n) == -1) {
			perror("write");
			return;
		}
	}

	// закрыть файл
	close(fd);
}
