#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
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

int main(void) {

	extern int errno;
	struct dirent *dirbuf;
	DIR *fd;
	
	int size_names = 5;
	int n = 0;
	char *p;

	// массив указателей
	char **names;
	
	// динамически выделяю память для 5 указателей на имена
	if ((names = (char **) calloc(size_names, sizeof(char *))) == NULL) {
		// fprintf(stderr, "%s\n", strerror(errno));
		perror("calloc");
		return -1;
	}

	// получаю структуру с файловым дескриптором и структурой первого файла: имя - номер индекса
	if ((fd = opendir(".")) == NULL) { 
		fprintf(stderr, "can't open directory\n");
		return 1;
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
		
		// выделяю память динамически для каждого имени файла
		if ((p = (char *) malloc(strlen(dirbuf->d_name) + 1)) == NULL) {
			// fprintf(stderr, "%s\n", strerror(errno));
			perror("malloc");
			return -1;
		}
		
		// записываю имя файла в буфер имен
		strcpy(p, dirbuf->d_name);

		// проверяю есть ли место в массиве указателей для следующего указателя
		if (n == size_names - 1) {
			// выделяю дополнительный блок памяти большего размера с сохранением в него текущего массива указателей
			// записываю указатель на имя файла в выделенную динамически память на указатели
			size_names *= 2;
			if ((names = (char **) realloc(names, size_names * sizeof(char *))) == NULL) {
				// fprintf(stderr, "%s\n", strerror(errno));
				perror("realloc");
				return -1;
			}
		}
		
		// записываю указатель на имя файла в выделенную динамически память на указатели
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
	
	return 0;
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
