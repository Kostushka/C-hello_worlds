#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("invalid value for request\n");
		return -1;
	}

	struct sockaddr_in server;

	// заполнить все поля структуры нулями
	memset(&server, 0, sizeof(server));

	// протокол
	server.sin_family = AF_INET;
	// порт
	server.sin_port = htons(5000);
	// IP адрес
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	// файловый дескриптор сокета
	int sockfd;
	
	// создаем сокет
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket");
		return -1;
	}
	
	// устанавливаем соединение с сервером
	if ((connect(sockfd, (struct sockaddr *) &server, sizeof(server))) == -1) {
		perror("connect");
		return -1;
	}
	
	char buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	
	// открываю файл с запросом
	int fd = open(argv[1], O_RDONLY, 0);
	int n;

	// читаю файл с запросом в буфер
	while ((n = read(fd, buf, BUFSIZ)) != 0) {
		if (n == -1) {
			fprintf(stderr, "%s: %s", argv[1], strerror(errno));
			break;
		}
		
		// записываю запрос в сокет
		if (write(sockfd, buf, n) == -1) {
			perror("write");
		}	
	}

	// закрываю файл с запросом
	close(fd);

	// читаю ответ из сокета в буфер
	while ((n = read(sockfd, buf, BUFSIZ)) != 0) {
		if (n == -1) {
			perror("read");
			break;
		}

		// вывожу ответ из буфера в stdout
		if (write(1, buf, n) == -1) {
			perror("write");
		}
	}

	return 0;
}
