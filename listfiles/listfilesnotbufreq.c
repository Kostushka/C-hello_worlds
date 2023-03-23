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
	char **names;
	int size_names;
	int num_names;
};

int mystrcmp(char *s, char *t);
void myqsort(char *s[], int start, int end);
void swap(char *s[], int i, int j);
int listfiles(char *dirname, struct Names *pp);

int main(void) {

	// указатель на структуру
	struct Names *pp;
	if ((pp = (struct Names *) malloc(sizeof(struct Names))) == NULL) {
		perror("malloc");
		return 1;
	}
	
	// массив указателей на имена
	char **names = NULL;

	pp->names = names;
	pp->size_names = 5;
	pp->num_names = 0;

	if ((names = (char **) malloc(pp->size_names * sizeof(char *))) == NULL) {
		perror("malloc");
		return 1;
	}

	// передаю указатель на переменную, которая хранит указатель на блок памяти
	// &names - адрес переменной, не блока памяти!!!
	// &size_names, &num_names - передаю адреса переменных, чтобы получать измененные значения
	if (listfiles(".", pp) == -1) {
		return 1;
	}

	// сортирую буфер с указателями на имена в алфавитном порядке
	myqsort(names, 0, pp->num_names - 1);

	// вывожу каждое имя из буфера с указателями на имена
	for (int i = 0; i < pp->num_names; i++) {
		printf("%s\n", names[i]);
		// очищаю память, выделенную под имена
		free(names[i]);
	}
	free(names);
	
	return 0;
}

int listfiles(char *dirname, struct Names *pp) {

	extern int errno;
	struct dirent *dirbuf;
	DIR *fd;
	struct stat stbuf;	

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
		
		if (pp->num_names == pp->size_names - 1) {
			pp->size_names *= 2;
			if ((pp->names = (char **) realloc(pp->names, pp->size_names * sizeof(char *))) == NULL) {
				perror("realloc");
				return -1;
			}
		}
		fprintf(stderr, "%lu %d %d\n", sizeof(pp->names[0]), pp->size_names, pp->num_names);
		// записываю указатель на имя файла в буфер указателей на имена
		pp->names[pp->num_names++] = path;
		fprintf(stderr, "%p %d %d\n", pp->names[0], pp->size_names, pp->num_names);


		// получаем информацию из inode по имени файла и записываем в буфер
		if (stat(path, &stbuf) == -1) {
			fprintf(stderr, "%s: %s\n", path, strerror(errno));
		}

		// проверяем по данным из inode, что файл - каталог
		if (S_ISDIR(stbuf.st_mode)) {
			// рекурсивно вызываем функцию, передаем относительный путь: например, ./d1/
			listfiles(path, pp);	
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
