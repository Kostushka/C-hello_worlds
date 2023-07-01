#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "labyrinth.h"

int main(int argc, char *argv[]) {
	int fd;

	// проверить корректность параметров программы
	if (argc != 3) {
		printf("incorrect input of program args\n");
		return 1;
	}

	// получить размер файла конфигурации
	struct stat stbuf;
	if (stat(argv[1], &stbuf) == -1) {
		fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
		return 1;
	}

	// открыть файл конфигурации
	if ((fd = open(argv[1], O_RDONLY, 0)) == -1) {
		fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
		return 1;
	}
	
	struct Labyrinth *lab;
	// функция парсинга файла конфигурации
	if ((lab = init_labyrint(fd, stbuf.st_size)) == NULL) {
		close(fd);
		return 1;
	}

	// закрыть файл конфигурации
	close(fd);

	FILE *fp;
	// открыть файл с командами
	if ((fp = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "%s: %s\n", argv[2], strerror(errno));
		destroy_lab(lab);
		return 1;
	}
	
	// создать хэш для имен команд и их обработчиков
	struct Hash *command_data = hash_init();
	if (command_data == NULL) {
		destroy_lab(lab);
		fclose(fp);
		return 1;
	}
	
	// функция обработки файла команд
	if (handling_command(fp, lab, command_data) == NULL) {
		destroy_lab(lab);
		fclose(fp);
		return 1;
	}

	// закрыть файл с командами
	fclose(fp);
	
	// очистить память, выделенную под лабиринт
	destroy_lab(lab);
	// очистить память, выделенную под хэш
	hash_destroy(command_data);

	return 0;
}
