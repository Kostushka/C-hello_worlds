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

int mystrcmp(char *s, char *t);
void myqsort(char *s[], int start, int end);
void swap(char *s[], int i, int j);
void listfiles(char *dirname, char **names);

int size_names = 5;
int n = 0;

int main(void) {

	// массив указателей на имена
	char **names;

	if ((names = (char **) malloc(size_names * sizeof(char *))) == NULL) {
		perror("malloc");
		return 1;
	}
	
	listfiles(".", names);

	// сортирую буфер с указателями на имена в алфавитном порядке
	myqsort(names, 0, n - 1);

	// вывожу каждое имя из буфера с указателями на имена
	for (int i = 0; i < n; i++) {
		printf("%s\n", names[i]);
		// очищаю память, выделенную под имена
		free(names[i]);
	}
	free(names);
	
	return 0;
}

void listfiles(char *dirname, char **names) {
	
	extern int errno;
	struct dirent *dirbuf;
	DIR *fd;
	struct stat stbuf;	
	char *p;

	// получаю структуру с файловым дескриптором и структурой первого файла: имя - номер индекса
	if ((fd = opendir(dirname)) == NULL) { 
		fprintf(stderr, "%s: %s\n", dirname, strerror(errno));
		exit(1);
	}

	// пока есть пункты списка, работаю с каждым из них
	while ((dirbuf = readdir(fd)) != NULL) {
	
		if (strcmp(dirbuf->d_name, ".") == 0) {
			continue;
		} 
		if (strcmp(dirbuf->d_name, "..") == 0) {
			continue;
		} 
		if (dirbuf->d_name[0] == '.') {
			continue;
		}

		// выделяем буфер под путь, чтобы его менять (т.к. строка по указателю доступна только для чтения)
		char *path;
		if ((path = (char *) malloc(strlen(dirname) + strlen("/") + 1 + strlen(dirbuf->d_name) + 1 + strlen("/") + 1)) == NULL) {
			perror("malloc");
			return;
		}
		
		// записываем имя в выделенный буфер + /: например ./
		strcpy(path, dirname);
		strcat(path, "/");

		// добавляем к имени каталога имя содержащегося в нем файла: например, ./d1
		strcat(path, dirbuf->d_name);

		// выделяю память динамически для имени каждого файла
		if ((p = (char *) malloc(strlen(path) + 1)) == NULL) {
			perror("malloc");
			return;
		}
		strcpy(p, path);
		
		if (n == size_names - 1) {
			size_names *= 2;
			if ((names = (char **) realloc(names, size_names * sizeof(char *))) == NULL) {
				perror("realloc");
				return;
			}
		}

		// записываю указатель на имя файла в буфер указателей на имена
		names[n++] = p;

		// получаем информацию из inode по имени файла и записываем в буфер
		if (stat(path, &stbuf) == -1) {
			fprintf(stderr, "%s: %s\n", path, strerror(errno));
		}
		
		// проверяем по данным из inode, что файл - каталог
		if (S_ISDIR(stbuf.st_mode)) {
			printf("%s - каталог\n", path);
			// рекурсивно вызываем функцию, передаем относительный путь: например, ./d1/
			listfiles(path, names);	
		}	
		
		// очищаем память, выделенную под имя для формирования относительного пути
		printf("path: %s\n", path);
		free(path);
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
