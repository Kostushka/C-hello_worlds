#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parsinghttp.h"

// int socket(int domain, int type, int protocol);
// int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
// int listen(int sockfd, int backlog);
// int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);

// преобразование значений между хостовым и сетевым порядком байтов
// uint32_t htonl(uint32_t hostlong);
// uint16_t htons(uint16_t hostshort);

int main(void) {

	// структура с данными сокета сервера
	// struct sockaddr_in {
	   // short int            sin_family; - семейство протоколов создаваемого сокета: AF_INET для сетевого протокола IPv4
	   // unsigned short int   sin_port; - номер порта который намерен занять процесс
	   // struct in_addr       sin_addr; - содержит поле s_addr, 
	   // 								   которому можно присвоить 32х битное значение IP адреса: 
	   //                                  INADDR_ANY - все адреса локального хоста (0.0.0.0);
	   // unsigned char        sin_zero[8];
	// };

	struct sockaddr_in server;

	// заполнить все поля структуры нулями
	memset(&server, 0, sizeof(server));

	// семейство протоколов
	server.sin_family = AF_INET;
	// номер порта
	server.sin_port = htons(5000);
	// IP адрес
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	// файловый дескриптор сокета
	int sockfd;
	
	// создаем сокет
	// когда сокет создается при помощи socket(), он ассоциируется с некоторым семейством адресов, но не с конкретным адресом
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket");
		return -1;
	}

	// связывает сокет с конкретным адресом
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
		perror("bind");
		return -1;
	}
	

	// слушаем сокетные соединения (запросы) и определяем кол-во соединений в очереди
	if ((listen(sockfd, 10)) == -1) {
		perror("listen");
		return -1;
	}

	int sockfd_client;

	// char buf[BUFSIZ];
	// memset(buf, 0, BUFSIZ);

	while (1) {
		// принятие запроса на установление соединения, создание нового сокета для каждого соединения (конкретного клиента)
		sockfd_client = accept(sockfd, (struct sockaddr *) NULL, NULL);
		if (sockfd_client == -1) {
			perror("accept");
			return -1;
		}

		// создаем структуру для записи запроса
		struct Http_request *http = (struct Http_request *) malloc(sizeof(struct Http_request));
		if (http == NULL) {
			perror("malloc");
			return -1;
		}

		// чтение запроса в структуру
		readreq(sockfd_client, http);
		
		/*
		int n;
		
		// читаю в буфер из клиентского сокета запрос
		while ((n = read(sockfd_client, buf, BUFSIZ)) != 0) {
			if (n == -1) {
				perror("read");
				break;
			}
			
			// пишу в stdout из буфера запрос
			write(1, buf, n);
		}
		*/
		
	
		// закрываем файл сокета для клиента
		close(sockfd_client);
	}

	return 0;
}
