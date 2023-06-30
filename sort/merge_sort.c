#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LEN(x) (int) (sizeof(x) / sizeof(x[0]))

//                                  *8*[13, 3, 8, 1, 15, 2, 3, 7, 4]
//                                       0  1  2  3 |4|  5  6  7  8
//                               *4*[13, 3, 8, 1, 15]  *7*[2, 3, 7, 4]
//                                    0  1 |2| 3   4       5 |6| 7  8
//                            *2*[13, 3, 8] *3*[1, 15]    *5*[2, 3] *6*[7, 4]
//                                 0 |1| 2     |3|  4        |5| 6     |7| 8
//                          *1*[13, 3]  [8]   [1]  [15]      [2]  [3]  [7]  [4] 
//                             |0|  1    2     3     4        5    6    7    8
//                        [13] [3]
//                          0   1


void merge_sort(int *arr, int low, int high);
void merge(int *arr, int low, int high, int mid);
int main(void) {

	int arr[] = {13, 3, 8, 1, 15, 2, 3, 7, 4};
	int low = 0;
	int high = LEN(arr) - 1;
	for (int i = 0; i < LEN(arr); i++) {
		printf("%d\t", arr[i]);
	}
	putchar('\n');
	merge_sort(arr, low, high);
	return 0;
}

void merge_sort(int *arr, int low, int high) {
	// в сортируемом массиве должно быть больше одного элемента
	if (low == high) {
		return;
	}
	// находим индекс среднего элемента
	int mid = low + (high - low) / 2;
	// разбиваем массив на подмассивы, пока не дойдем до 1 элемента в массиве (в сортировке не нуждается)
	merge_sort(arr, low, mid);
	merge_sort(arr, mid + 1, high);
	printf("low: %d mid: %d high: %d\n", arr[low], mid, arr[high]);
	// сортируем и заменяем в исходном массиве отсортированным блоком элементов исходный блок
	merge(arr, low, high, mid);
	for (int i = low; i <= high; i++) {
		printf("%d\t", arr[i]);
	}
	putchar('\n');
}

void merge(int *arr, int low, int high, int mid) {
	// создать массив для отсортированных элементов
	int len = high - low + 1;
	// память выделяется в куче, ибо размер стека ограничен: 8Мб
	int *buf = (int *) malloc(sizeof(int) * len);
	if (buf == NULL) {
		perror("malloc\n");
		return;
	}
	
	// сортировка
	int i, j, k;
	i = low;
	j = mid + 1;
	k = 0;
	// пока в двух половинах есть элементы 
	while (i <= mid && j <= high) {
		// сравниваем
		if (arr[i] < arr[j]) {
			// добавляем меньший элемент в массив для отсортированных элементов
			buf[k++] = arr[i];
			i++;
		} else {
			buf[k++] = arr[j];
			j++;
		}
	}
	// пока в первой половине есть элементы, добавляем их
	while (i <= mid) {
		buf[k++] = arr[i++];
	}
	// пока во второй половине есть элементы, добавляем их
	while (j <= high) {
		buf[k++] = arr[j++];
	}
	
	// заменяем в исходном массиве отсортированным блоком элементов исходный блок
	memcpy(&arr[low], buf, len * sizeof(buf[0]));

	free(buf);
}

