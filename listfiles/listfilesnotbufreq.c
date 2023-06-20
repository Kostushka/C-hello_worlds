#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "listfilesnotbufreq.h"
#define SIZE 10

// Программа выводит отсортированный в алфавитном порядке список файлов в текущем каталоге

// пункт списка файлов
// Dirent {
	// char d_name[NAME_MAX + 1]; // имя файла
	// int d_ino; // файловый индекс
// };

// DIR {
	// int fd; // файловый дескриптор каталога
	// struct Dirent d; // пункт списка файлов
// };

struct Names {
	char **items;
	int size;
	int num;
};

int mystrcmp(char *s, char *t);
void myqsort(char *s[], int start, int end);
void swap(char *s[], int i, int j);
int listfiles(struct Hash *hash, char *dirname, struct Names *names);
char *get_correct_path(char *s);
void free_hash_struct(struct Hash *hash);
void hash_destroy(struct Hash *hash);

int main(int argc, char **argv) {

	// создаю хэш и записываю указатель на него
	struct Hash *hash = create_hash(SIZE);
	if (hash == NULL) {
		return -1;
	}
	
	// struct Hash hash;
	// 
    // if ((hash.hashtab = (struct Block **) malloc(sizeof(struct Block *) * SIZE)) == NULL) {
        // perror("malloc");
        // return -1;
    // }
    // hash.size_hashtab = SIZE;
    // hash.count_struct = 0;
	
	/*
	// исключаем пути с дублями файлов
	// массив с нужными путями
	char **paths;
	// кол-во нужных путей
	int length;
	// если есть аргументы
	if (argc > 1) {
		length = get_not_duplpaths(argv + 1, argc - 1, &paths);
	}
	*/

	// структура
	struct Names names;

	// максимальное кол-во указателей на имена
	names.size = 5;
	// записанное кол-во указателей на имена
	names.num = 0;

	// выделение памяти под массив с 5 указателями на имена
	if ((names.items = (char **) malloc(names.size * sizeof(char *))) == NULL) {
		perror("malloc");
		return 1;
	}

	char *pathname;

	// массив аргументов
	char **argvp;
	// копия argc для изменения данной переменной
	int argcopy = argc;
	
	// если нет аргументов, добавляем в аргументы "."
	if (argcopy == 1) {
		// объявляем массив указателей с одним указателем: "."
		char *default_arg[] = {"."};
		argvp = default_arg;
	} else {
		// если аргументы переданы программе, сдвигаем адрес с имени самой программы на первый аргумент
		// *argv[] = {"./a.out", "arg1", "arg2", ...}
		argvp = argv + 1;
		// argvp = paths;
		// учитываем, что argc включает в аргументы и имя самой программы
		--argcopy;
		// argcopy = length;
	}

	for (int i = 0; i < argcopy; i++) {
		// из каждого аргумента формируем корректный путь (без /, если он есть)
		char *correctpath = get_correct_path(argvp[i]);
		pathname = correctpath;
	
		// записываем в буфер структуры рекурсивно все указатели на имена файлов, полученные по переданным путям
		// &names - адрес структуры
		if (listfiles(hash, pathname, &names) == -1) {
			return 1;
		}
	}

	// сортирую буфер с указателями на имена в алфавитном порядке
	myqsort(names.items, 0, names.num - 1);

	for (int i = 0; i < names.num; i++) {
		printf("%s\n", names.items[i]);
		free(names.items[i]);
	}
	
	hash_destroy(hash);
	free(names.items);
		
	/*
	// исключаю дубликаты отсортированных указателей на имена при выводе
	// вывожу имена из буфера: прохожусь по всем именам кроме последнего
	for (int i = 0; i < names.num - 1; i++) {

		// сравниваю текущее имя со следующим: при совпадении текущее имя пропускаю
		// например, names.num = 5 => 0 1 2 3 4 => 0 1 | 1 2 | 2 3 | 3 4
		if (strcmp(names.items[i + 1], names.items[i]) != 0) {
			printf("%s\n", names.items[i]);
		}

		// очищаю память, выделенную под указатели на имена
		free(names.items[i]);
	}

	// последнее имя в буфере с указателями на имена должно выводиться всегда
	printf("%s\n", names.items[names.num - 1]);
	free(names.items[names.num - 1]);

	// очищаю память, выделенную под массив указателей
	free(names.items);
	*/
	
	return 0;
}

void hash_destroy(struct Hash *hash) {
	free_hash_struct(hash);
	free(hash->hashtab);
	free(hash);
}

void free_hash_struct(struct Hash *hash) {
	// проходимся по всем элементам массива структур
	for (int i = 0; i < hash->size_hashtab; i++) {
		// если по индексу нет структур
	    if (hash->hashtab[i] == NULL) {
	    	continue;
	    }
	    
	    // если по индексу одна структура или структура содержит указатель на другую структуру (связанный список)
	    struct Block *p = hash->hashtab[i];
	    while (p != NULL) {
	    	struct Block *temp = p;
	    	p = p->p;
	    	free(temp->key);
	    	free(temp);
	    }
	}
}

char *get_correct_path(char *s) {

	char *t = s;
	
	// *s - указывает на последний символ
	// работаю с локальной копией
	s += strlen(s) - 1;
	
	if (*s == '/') {
		*s = '\0';
	}
	
	return t;
}

int listfiles(struct Hash *hash, char *dirname, struct Names *names) {
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

		// if (is_written) {
			// continue;
		// }

		// если путь уже есть в хэше, то пропускаем работу с этим путем
		if (add_hash(hash, path) == 1) {
			continue;
		}

		// записываю указатель на имя файла в буфер указателей на имена
		names->items[names->num++] = path;	

		// получаем информацию из inode по имени файла и записываем в буфер
		if (stat(path, &stbuf) == -1) {
			fprintf(stderr, "%s: %s\n", path, strerror(errno));
		}

		// проверяем по данным из inode, что файл - каталог
		if (S_ISDIR(stbuf.st_mode)) {
			// рекурсивно вызываем функцию, передаем относительный путь: например, ./d1/
			listfiles(hash, path, names);	
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
