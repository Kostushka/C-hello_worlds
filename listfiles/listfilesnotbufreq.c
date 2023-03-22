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
void listfiles(char *filename);

int main(void) {

	listfiles(".");
	
	return 0;
}

void listfiles(char *filename) {
	printf("%s\n", filename);
	
	extern int errno;
	struct dirent *dirbuf;
	DIR *fd;
	struct stat stbuf;
	int size_names = 5;
	int n = 0;
	char *p;

	// массив указателей на имена
	char **names;

	if ((names = (char **) malloc(size_names * sizeof(char *))) == NULL) {
		perror("malloc");
		return;
	}

	// получаю структуру с файловым дескриптором и структурой первого файла: имя - номер индекса
	if ((fd = opendir(filename)) == NULL) { 
		fprintf(stderr, "%s: %s\n", filename, strerror(errno));
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

		// выделяем буфер под имя каталога, чтобы его менять (т.к. строка по указателю доступна только для чтения)
		char *filenamep;
		if ((filenamep = (char *) malloc(strlen(filename) + strlen("/") + 1 + strlen(dirbuf->d_name) + 1 + strlen("/") + 1)) == NULL) {
			perror("malloc");
			return;
		}
		
		// записываем имя в выделенный буфер
		strcpy(filenamep, filename);
		
		// если имя . добавляем / для формирования корректного относительного пути 
		if (strcmp(filenamep, ".") == 0) {
			strcat(filenamep, "/");
		}

		// добавляем к имени каталога имя содержащегося в нем файла: например, ./d1
		strcat(filenamep, dirbuf->d_name);
		
		printf("%s\n", filenamep);

		// получаем информацию из inode по имени файла и записываем в буфер
		if (stat(filenamep, &stbuf) == -1) {
			fprintf(stderr, "%s: %s\n", dirbuf->d_name, strerror(errno));
		}
		
		// проверяем по данным из inode, что файл - каталог
		if (S_ISDIR(stbuf.st_mode)) {
			// рекурсивно вызываем функцию, передаем относительный путь + /: например, ./d1/
			listfiles(strcat(filenamep, "/"));
		}

		// очищаем память, выделенную под имя для формирования относительного пути
		free(filenamep);
	
		// выделяю память динамически для каждого имени файла
		if ((p = (char *) malloc(strlen(dirbuf->d_name) + 1)) == NULL) {
			perror("malloc");
			return;
		}
		
		// записываю имя файла в буфер имен
		strcpy(p, dirbuf->d_name);
		
		if (n == size_names - 1) {
			size_names *= 2;
			if ((names = (char **) realloc(names, size_names * sizeof(char *))) == NULL) {
				perror("realloc");
				return;
			}
		}

		// записываю указатель на имя файла в буфер указателей на имена
		names[n++] = p;

	}

	// закрываю структуру каталога
	closedir(fd);

	// сортирую буфер с указателями на имена в алфавитном порядке
	myqsort(names, 0, n - 1);

	// вывожу каждое имя из буфера с указателями на имена
	for (int i = 0; i < n; i++) {
		printf("%s  ", names[i]);
		// очищаю память, выделенную под имена
		free(names[i]);
	}
	free(names);
	putchar('\n');
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
