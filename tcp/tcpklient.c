#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

// int socket(int domain, int type, int protocol);
// int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);

// uint32_t htonl(uint32_t hostlong);
// uint16_t htons(uint16_t hostshort);

int main(void) {

	// структура с данными сервера
	// struct sockaddr_in {
	   // short int            sin_family;
	   // unsigned short int   sin_port; - номер порта который намерен занять процесс
	   // struct in_addr       sin_addr; - содержит поле s_addr, 
	   // 								   которому можно присвоить 32х битное значение IP адреса: 
	   //                                  INADDR_ANY - все адреса локального хоста (0.0.0.0);
	   // unsigned char        sin_zero[8];
	// };
	struct sockaddr_in klient;

	// заполнить все поля структуры нулями
	memset(&klient, '0', sizeof(klient));

	// протокол
	klient.sin_family = AF_INET;
	// порт
	klient.sin_port = htons(5000);
	// IP адрес
	klient.sin_addr.s_addr = inet_addr("127.0.0.1");

	// файловый дескриптор сокета
	int sockfd;
	
	// создаем сокет
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket");
		return -1;
	}

	// устанавливаем соединение с сервером
	if ((connect(sockfd, (struct sockaddr *) &klient, sizeof(klient))) == -1) {
		perror("connect");
		return -1;
	}

	char buf[1024];
	memset(buf, '0', sizeof(buf));
	
	int n;

	// читаем в буфер данные из сокета, пока они есть
	while ((n = read(sockfd, buf, sizeof(buf) - 1)) > 0) {
		if (n == -1) {
			perror("read");
			return -1;
		}
		// ???
		buf[n] = 0;
		
		if (write(1, buf, n) == -1) {
			perror("write");
		}
	}

	return 0;
}
