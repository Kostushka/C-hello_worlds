#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parsinghttp.h"

#define GET 1	   // 00000001
#define POST 2     // 00000010
#define PATCH 4    // 00000100
#define PUT 8      // 00001000
#define DELETE 16  // 00010000

char method_get(struct Http_request *);

int main(void) {

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

		// путь до файла с HTTP-ответом	
		char *status = "/home/nastya/C-hello_worlds/http/response.txt";
		
		// путь до запрашиваемого файла, если resource: /
		char *home_file = "/home/nastya/C-hello_worlds/http/home.html";

		// ----------------------------------------------------------
		// ЗАПИСЬ ДАТЫ В ФАЙЛ HTTP-ОТВЕТА
		// ----------------------------------------------------------
		
		// получаем дату
		time_t t = time(NULL);
		
		// открываем файл с HTTP-ответом для чтения и записи
		int fd_status = open(status, O_RDWR, 0);
		if (fd_status == -1) {
			fprintf(stderr, "%s: %s\n", status, strerror(errno));
		}
		
		char c;
		char buf[BUFSIZ];
		int n, count = 0;
		// читаем файл посимвольно
		while ((n = read(fd_status, &c, 1)) != 0) {
			if (n == -1) {
				perror("read");
				return -1;
			}
			
			if (c == 'D') {
				while (c != ':') {
					// записываем в буфер все символы с D до :
					buf[count++] = c;
					if (read(fd_status, &c, 1) == -1) {
						perror("read");
						return -1;
					}
				}
				// если в буфере "Date"
				if (strcmp(buf, "Date") == 0) {
					// смещаемся на символ пробела для записи даты
					if (lseek(fd_status, 1, 1) == -1) {
						perror("lseek");
						return -1;
					}
					// записываем дату в файл с HTTP-ответом
					if (write(fd_status, ctime(&t), strlen(ctime(&t))) == -1) {
						perror("write");
						return -1;
					}
					break;
				}
			}
		}
		
		// закрываем файл с HTTP-ответом
		close(fd_status);
		
		// ----------------------------------------------------------

		// получить идентификатор метода в запросе
		char method = method_get(http);
			
		switch(method) {
			case GET:
				// запрос за файлом главной страницы
				if (strcmp(http->resource, "/") == 0) {
					// записываю в сокет файл со статусом HTTP-ответа
					writeres(status, sockfd_client);

					// записываю в сокет файл домашней страницы
					writeres(home_file, sockfd_client);				
				} else {
					// записываю в сокет файл со статусом HTTP-ответа
					writeres(status, sockfd_client);
					
					// записываю в сокет запрашиваемый файл 
					writeres(http->resource, sockfd_client);
				}
				break;
			case POST:
				break;
			case PUT:
				break;
			case PATCH:
				break;
			case DELETE:
				break;
		}

		// закрываем файл сокета для клиента
		close(sockfd_client);
	}

	return 0;
}

char method_get(struct Http_request *http) {
	char method_flag = 0;

	if (strcmp(http->method, "GET") == 0) {
		method_flag = GET;
	} 

	if (strcmp(http->method, "POST") == 0) {
		method_flag = POST;
	}

	if (strcmp(http->method, "PATCH") == 0) {
		method_flag = PATCH;
	}

	if (strcmp(http->method, "PUT") == 0) {
		method_flag = PUT;
	}

	if (strcmp(http->method, "DELETE") == 0) {
		method_flag = DELETE;
	}

	return method_flag;
}
