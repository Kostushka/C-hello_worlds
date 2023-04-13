#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int read_file_to_fd(char *filename, int file_fd, int fd);

int main(int argc, char *argv[]) {
	int file_fd, bytes;
	int res = 0;
	
	for (int i = 1; i < argc; i++) {
		// открываем файл для чтения
		file_fd = open(argv[i], O_RDONLY, 0);
		
		if (file_fd == -1) {
			fprintf(stderr, "%s: %s\n", argv[i], strerror(errno));
		} else {
			// получаем кол-во записанных байт
			if ((bytes = read_file_to_fd(argv[i], file_fd, 1)) != -1) {
				res += bytes;
			}
			// закрываем файд
			close(file_fd);
		}
	}
	printf("read and write %d bytes\n", res);

	return 0;
}

int read_file_to_fd(char *filename, int file_fd, int fd) {
	int n, w;
	int sum = 0;
	
	// буфер для считываемых символов
	char buf[BUFSIZ];
	
	// читаем файл в буфер
	while ((n = read(file_fd, buf, BUFSIZ)) != 0) {
		if (n == -1) {
			fprintf(stderr, "%s: %s\n", filename, strerror(errno));
			return -1;
		}	

		w = write(fd, buf, n);
		
		if (w == -1) {
			fprintf(stderr, "%s: %s\n", filename, strerror(errno));
			return -1;
		}
		
		sum += w;		
	}

	return sum;
}
