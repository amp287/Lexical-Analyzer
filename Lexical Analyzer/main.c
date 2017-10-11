#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "Symbols.h"

typedef struct TOKEN{
	int type;
	char value[10];
	struct TOKEN *next;
}TOKEN;

char token;
FILE *fp;
int quit;
char ident_buffer[IDENT_MAX_LENGTH + 1];
char *code;
int code_length;
TOKEN *start, *end;

int line;
int col;

int ident_or_reserved();
void read_file(FILE *fp);
void print_code();
void add_to_lexeme(int type, int lex);
int number();
int get_token();
void remove_comments();
int is_symbol();

int main(int argc, char *argv[]) {
	line = 0;
	col = 0;
	quit = 0;
	if (argc < 2) {
		printf("ERROR: PL/0 file must be provided in arguments!\n");
		return -1;
	}

	fp = fopen(argv[1], "r");

	read_file(fp);

	if (get_token()) {
		printf("Error File Empty!\n");
		return -1;
	}

	while (!quit) {
		if (isalpha(token)) {				//ident or reserved
			ident_or_reserved();
		} else if (isdigit(token)) {		//number

		} else if (token == ' ' || token == '\t' || token == '\n') {
			get_token();
			continue;
		} else if (token == '/') {
			remove_comments();
		} else {
			if (is_symbol() == 0) {			//Symbol

			}
		}
		memset(ident_buffer, 0, IDENT_MAX_LENGTH + 1);
	}


	print_code();

return 0;
}

//TODO: Add in <= >= etc...
int is_symbol() {
	int i;

	for (i = 0; i < NUM_SYMBOLS; i++) {
		if (symbols[i] == token) {
			if (token == ':') {
				if (get_token() == -1)
					return 0;
				if (token == '=') {
					//put becomes on stack
					add_to_lexeme(1, 13);
					return 0;
				} else {
					printf("Error : is not a valid symbol. %d:%d", line, col);
					quit = 1;
					return -1;
				}
			}
			//put symbol on stack
			add_to_lexeme(1, i);
			return 0;
		}
	}
	return -1;
}

void print_code(){
	int i;
	for(i = 0; i < code_length; i++) {
		printf("%c", code[i]);
	}
}

void remove_comments(){
	int stop = 0;
	int type = 0;

	if (get_token() == -1) {
		add_to_lexeme(1, 7);
		return;
	}

	if (token != '/' && token != '*') {
		add_to_lexeme(1, 7); // add slash to lexeme not part of comments
		get_token();
		return;
	}

	if (token == '/')
		type = 1;
	if (token == '*')
		type = 2;

	while (!stop) {
		if (get_token() == -1)
			return;

		if (type == 1 && token == '\n')
			break;
		if (type == 2 && token == '*') {
			if (get_token() == -1)
				return;
			if (token == '/')
				break;
		}
	}

	get_token();
}

int get_token() {
	if (fscanf(fp, "%c", &token) == EOF) {
		quit = 1;
		return -1;
	}

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

void read_file(FILE *fp) {
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

int number(){
	int length = 1;

	ident_buffer[0] = token;

	//if end of file just assume what we have is an number
	if (get_token() == -1) {
		goto NUMBER_EXIT;
	}

	 while(length < IDENT_MAX_LENGTH) {
		 if (isdigit(token)) {
			ident_buffer[length++] = token;
			get_token();
			continue;
		}
		 break;
	}

	 if (length == IDENT_MAX_LENGTH) {
		 get_token();
		 if (isdigit(token)) {
			 printf("Error number (%s) exceeds max size... %d:%d", ident_buffer, line, col);
			 return -1;
		}
	 }

NUMBER_EXIT:
	 add_to_lexeme(3, 0);
	 return 0;
}

int check_reserved() {
	int i;

	for (i = 0; i < NUM_RESERVED; i++) {
		if (strcmp(reserved[i], ident_buffer) == 0) {
			return i;
		}
	}
	return -1;
}

int ident_or_reserved() {
	int ret = 0;
	int length = 1;

	ident_buffer[0] = token;

	//if end of file just assume what we have is an ident
	if (get_token() == -1) {
		add_to_lexeme(2, 2);
		return 0;
	}

	while(length < IDENT_MAX_LENGTH) {
		if(isalpha(token) || isdigit(token)) {
			ident_buffer[length++] = token;
			if (get_token() == -1) {
				goto IDENT_RES_EXIT;
			}
			continue;
		}
		break;
	}

	if (length == IDENT_MAX_LENGTH) {
		get_token();
		if (isalpha(token)) {
			printf("Error identifier (%s) exceeds max size... %d:%d", ident_buffer, line, col);
			return -1;
		}
	}

IDENT_RES_EXIT:
	if ((ret = check_reserved()) != -1) {
		//put reserved on stack
		add_to_lexeme(0, ret);
	} else {
		//its an identifier
		add_to_lexeme(2, 2);
	}

	return 0;
}

//type: 0 = Reserved word
//type: 1 = Symbol lex = index in symbol_lex
//type: 2 = identifier lex = Anyvalue
//type: 3 = number
void add_to_lexeme(int type, int lex) {
	TOKEN *tok;
	tok = malloc(sizeof(TOKEN));
	if (type == 0) {
		tok->type = reserved_lex[lex];
	} else if (type = 1) {
		tok->type = symbol_lex[lex];
	} else if (type == 2) {
		tok->type = ident_sym;
		strcpy(tok->value, ident_buffer);
	}


	if (start == NULL) {
		start = tok;
		end = tok;
	} else {
		end->next = tok;
		end = tok;
	}
}