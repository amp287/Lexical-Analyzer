#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "Symbols.h"

typedef struct {
	int type;
	char value[10];
	TOKEN *next;
}TOKEN;

char token;
FILE *fp;
int quit;
char ident_buffer[IDENT_MAX_LENGTH];
char *code;
int code_length;
TOKEN *start, *end;

int line;
int col;

int ident_or_reserved();
int read_file(FILE *fp);
void print_code();

int main(int argc, char *argv[]) {
	line = 0;
	col = 0;
	quit = 0;
	/*if (argc < 2) {
		printf("ERROR: PL/0 file must be provided in arguments!");
		return -1;
	}*/

	//fp = fopen(argv[1], "r");

	//read_file(argv[1], strlen(argv[1]));
	fp = fopen(argv[1], "r");

	//read_file(fp);

	if (get_token()) {
		printf("Error File Empty!");
		return -1;
	}

	while (!quit) {
		if (isalpha(token)) {

		} else if (isdigit(token)) {

		} else {
			if (is_symbol(token)) {

			}
		}
	}


	print_code();

return 0;
}

int is_symbol(token) {
	int i;
	for (i = 0; i < NUM_SYMBOLS; i++) {
		if (reserved[i] == token) {
			if (token == ':') {
				get_token();
				if (token == '=') {
					//put begins on stack
					return 0;
				} else {
					printf("Error : is not a valid symbol. %d:%d", line, col);
					quit = 1;
					return -1;
				}
			}
			//put symbol on stack
		}
	}
}

void print_code(){
	int i;
	for(i = 0; i < code_length; i++) {
		printf("%c", code[i]);
	}
}

void remove_comments(){

}

int get_token() {
	if (fscanf(fp, "%c", &token) == EOF)
		return -1;

	if (token == '\n') {
		line++;
		col = 0;
	} else if ('\t') {
		col += 5;
	} else {
		col++;
	}

	return 0;
}

int read_file(FILE *fp) {
	char buffer;
	int count = 0;

	code = calloc(500, sizeof(char));
	code_length = 0;

	while(fscanf(fp, "%c", &buffer) != EOF){
		if(count >= code_length){
			code = realloc(code, code_length + 500);
			code_length += 500;
		}
		code[count] = buffer;
		count++;
	}
	code_length = count;
}

int number(char *input, int length, int state){
	if(length == 0)
		return 1;

		if(isdigit(*input))
			return number(++input, --length, 0);
		else
			return 0;
}

int ident_or_reserved() {
	int ret = 0;
	int length = 1;

	ident_buffer[0] = token;

	get_token();

	while(length < IDENT_MAX_LENGTH) {
		if(isalpha(token) || isdigit(token)) {
			ident_buffer[length++] = token;
			get_token();
			continue;
		}
		break;
	}

	if (check_reserved() == 0) {
		//put reserved on stack
	} else {
		//its an identifier
	}

	return 0;
}

int add_to_lexeme(int type) {
	if(type == )
}