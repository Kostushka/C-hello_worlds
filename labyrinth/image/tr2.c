#include <stdio.h>

int main(int argc, char *argv[]) {
	FILE *fp;
	if ((fp = fopen(argv[1], "r")) == NULL) {
		perror("fopen");
		return 1;
	}
	char buf[64];
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		for (int i = 0; i < 64; i++) {
			if (buf[i] == 0) {
				printf("break\n");
				break;
			}
			printf("%d\n", buf[i]);
		}
	}
	fclose(fp);
	
	return 0;
}
