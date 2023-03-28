#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>


// Программа выводит отсортированный в алфавитном порядке список файлов в текущем каталоге

// пункт списка файлов
// struct DIRENT {
	// char d_name[NAME_MAX + 1]; // имя файла
	// int d_ino; // файловый индекс
// };

// struct DIR {
	// int fd; // файловый дескриптор каталога
	// struct DIRENT d; // пункт списка файлов
// };

struct Names {
	char **items;
	int size;
	int num;
};

int mystrcmp(char *s, char *t);
void myqsort(char *s[], int start, int end);
void swap(char *s[], int i, int j);
int listfiles(char *dirname, struct Names *names);
char *getCorrectPath(char *s);

int main(int argc, char **argv) {

	// структура
	struct Names names;

	names.size = 5;
	names.num = 0;

	if ((names.items = (char **) malloc(names.size * sizeof(char *))) == NULL) {
		perror("malloc");
		return 1;
	}

	char *pathname;

	char **argvp;
	int argcopy = argc;
	
	// если нет аргументов, добавляем в аргументы "."
	if (argcopy == 1) {
		char *default_arg[] = {"."}; 
		argvp = default_arg;
	} else {
		argvp = argv + 1;
		// учитываем, что argc включает в аргументы и имя самой программы
		--argcopy;
	}

	for (int i = 0; i < argcopy; i++) {
		char *correctpath = getCorrectPath(argvp[i]);
		pathname = correctpath;
	
		// &names - адрес структуры
		if (listfiles(pathname, &names) == -1) {
			return 1;
		}
	}
	
	// сортирую буфер с указателями на имена в алфавитном порядке
	myqsort(names.items, 0, names.num - 1);

	// вывожу каждое имя из буфера с указателями на имена
	for (int i = 0; i < names.num; i++) {

		if ((i + 1 == names.num) || (strcmp(names.items[i + 1], names.items[i]) != 0)) {
			printf("%s\n", names.items[i]);	
		}

		// очищаю память, выделенную под имена
		free(names.items[i]);
	}
	free(names.items);
	
	return 0;
}

char *getCorrectPath(char *s) {

	char *t = s;
	
	// *s - указывает на последний символ
	s += strlen(s) - 1;
	
	if (*s == '/') {
		*s = '\0';
	}
	
	return t;
}

int listfiles(char *dirname, struct Names *names) {

	extern int errno;
	struct dirent *dirbuf;
	DIR *fd;
	struct stat stbuf;

	// char is_written = 0;
	
	// получаю структуру с файловым дескриптором и структурой первого файла: имя - номер индекса
	if ((fd = opendir(dirname)) == NULL) { 
		fprintf(stderr, "%s: %s\n", dirname, strerror(errno));
		return -1;
	}

	// пока есть пункты списка, работаю с каждым из них
	while ((dirbuf = readdir(fd)) != NULL) {

		if (dirbuf->d_name[0] == '.') {
			continue;
		}

		// выделяем буфер под путь, чтобы его менять (т.к. строка по указателю доступна только для чтения)
		char *path;
		if ((path = (char *) malloc(strlen(dirname) + strlen("/") + strlen(dirbuf->d_name) + strlen("/") + 1)) == NULL) {
			perror("malloc");
			return -1;
		}
		
		// записываем имя в выделенный буфер + /: например ./
		strcpy(path, dirname);
		strcat(path, "/");

		// добавляем к имени каталога имя содержащегося в нем файла: например, ./d1
		strcat(path, dirbuf->d_name);
		
		if (names->num == names->size - 1) {
			names->size *= 2;
			if ((names->items = (char **) realloc(names->items, names->size * sizeof(char *))) == NULL) {
				perror("realloc");
				return -1;
			}
		}

		// проверяем, записан ли уже этот путь в массив имен
		// for (int i = 0; i < names->num; i++) {
			// if (strcmp(path, names->items[i]) == 0) {
				// is_written = 1;
			// }
		// }
// 
		// if (is_written) {
			// continue;
		// }
	
		// записываю указатель на имя файла в буфер указателей на имена
		names->items[names->num++] = path;	

		// получаем информацию из inode по имени файла и записываем в буфер
		if (stat(path, &stbuf) == -1) {
			fprintf(stderr, "%s: %s\n", path, strerror(errno));
		}

		// проверяем по данным из inode, что файл - каталог
		if (S_ISDIR(stbuf.st_mode)) {
			// рекурсивно вызываем функцию, передаем относительный путь: например, ./d1/
			listfiles(path, names);	
		}	
	}

	// закрываю структуру каталога
	closedir(fd);
}


void myqsort(char *s[], int start, int end) {
	int pivot, i;
	if (start >= end) {
		return;
	}
	swap(s, start, (start + end) / 2);
	pivot = start;
	for (i = start + 1; i <= end; i++) {
		if (mystrcmp(s[start], s[i]) > 0) {
			swap(s, ++pivot, i);
		}
	}
	swap(s, start, pivot);
	myqsort(s, start, pivot - 1);
	myqsort(s, pivot + 1, end);
	
}

void swap(char *s[], int i, int j) {
	char *c;
	c = s[i];
	s[i] = s[j];
	s[j] = c;
}

// tolower: чтобы если имя файла начинается с символа в верхнем регистре, перевести его в нижний регистр
int mystrcmp(char *s, char *t) {
	while (tolower(*s) == tolower(*t)) {
		if (*s == '\0') {
			return 0;
		}
		++s;
		++t;
	}
	return tolower(*s) - tolower(*t);
}
