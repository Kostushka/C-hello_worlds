#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

// int socket(int domain, int type, int protocol);
// int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);

// uint32_t htonl(uint32_t hostlong);
// uint16_t htons(uint16_t hostshort);

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("invalid value for request\n");
		return -1;
	}

	// структура с данными сервера
	// struct sockaddr_in {
	   // short int            sin_family;
	   // unsigned short int   sin_port; - номер порта который намерен занять процесс
	   // struct in_addr       sin_addr; - содержит поле s_addr, 
	   // 								   которому можно присвоить 32х битное значение IP адреса: 
	   //                                  INADDR_ANY - все адреса локального хоста (0.0.0.0);
	   // unsigned char        sin_zero[8];
	// };
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

		buf[n] = 0;
		
		// записываю запрос в сокет
		write(sockfd, buf, n);	
	}

	// закрываю файл с запросом
	close(fd);

	// устанавливаем соединение с сервером
	if ((connect(sockfd, (struct sockaddr *) &server, sizeof(server))) == -1) {
		perror("connect");
		return -1;
	}

	return 0;
}
