#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#define MAX 100


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

// буфер с указателями на имена файлов
char *s[MAX];

int main(void) {

	struct dirent *dirbuf;
	DIR *fd;
	int n = 0;
	char *p;

	// получаю структуру с файловым дескриптором и структурой первого файла: имя - номер индекса
	if ((fd = opendir(".")) == NULL) { 
		fprintf(stderr, "can't open directory\n");
		return 1;
	}

	// пока есть пункты списка, работаю с каждым из них
	while ((dirbuf = readdir(fd)) != NULL) {
	
		if (strcmp(dirbuf->d_name, ".") == 0) {
			continue;
		} else if (strcmp(dirbuf->d_name, "..") == 0) {
			continue;
		} else if (dirbuf->d_name[0] == '.') {
			continue;
		} else {
			// выделяю память динамически для каждого имени файла
			if ((p = (char *) malloc(strlen(dirbuf->d_name) + 1)) == NULL) {
				fprintf(stderr, "too much files\n");
				return -1;
			}
			
			// записываю имя файла в буфер имен
			strcpy(p, dirbuf->d_name);
			// записываю указатель на имя файла в буфер указателей на имена
			s[n++] = p;
		}
		
	}

	// закрываю структуру каталога
	closedir(fd);

	// сортирую буфер с указателями на имена в алфавитном порядке
	myqsort(s, 0, n - 1);

	// вывожу каждое имя из буфера с указателями на имена
	for (int i = 0; i < n; i++) {
		printf("%s  ", s[i]);
		// очищаю память, выделенную под имена
		free(s[i]);
	}
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
