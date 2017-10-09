#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
	int type;
	char value[10];

}TOKEN;

int main(int argc, char *argv[]) {
	FILE *fp;

	if (argc < 2) {
		printf("ERROR: PL/0 file must be provided in arguments!");
		return -1;
	}

	fp = fopen(argv[1], "r");




}

int read_file(FILE *fp) {

}