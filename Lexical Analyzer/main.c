//Austin Peace
//Sebastian De La Cruz
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "Symbols.h"

typedef struct TOKEN {
	int type;
	char value[10];
	struct TOKEN *next;
}TOKEN;

char token;
FILE *fp;
int quit;
char ident_buffer[IDENT_MAX_LENGTH + 1];
char *code;
int code_length, code_alloc;
TOKEN *start, *end;

int line;
int col;
int prev_col;
int prev_line;

int ident_or_reserved();
void print_lexeme_list();
void add_to_lexeme(int type, int lex);
void print_lexeme_table();
int number();
int get_token();
void remove_comments();
int is_symbol();
void free_tokens();
void print_code();

int main(int argc, char *argv[]) {
	int error = 0;

	line = 1;
	col = 0;
	quit = 0;

	if (argc < 2) {
		printf("Error: PL/0 file must be provided in arguments!\n");
		return -1;
	}

	fp = fopen(argv[1], "r");

	if (fp == NULL) {
		printf("Error: %s does not exist!", argv[1]);
		return -1;
	}

	code_alloc = 500;
	code_length = 0;
	code = calloc(500, 1);

	if (get_token()) {
		printf("Error: File Empty!\n");
		return -1;
	}

	while (!quit) {
		if (isalpha(token)) {						//ident or reserved
			if(ident_or_reserved())
				goto MAIN_EXIT;
		} else if (isdigit(token)) {		//number
			if(number())
				goto MAIN_EXIT;
		} else if (token == ' ' || iscntrl(token)) {
			if(get_token())
				quit = 1;
			continue;
		} else if (token == '/') {
			remove_comments();
		} else {
			if (is_symbol())							//Symbol
				goto MAIN_EXIT;
		}
		memset(ident_buffer, 0, IDENT_MAX_LENGTH + 1);
	}

	print_code();
	print_lexeme_table();
	print_lexeme_list();
MAIN_EXIT:
	free_tokens();
	free(code);

return 0;
}

int is_symbol() {
	int i, is_single = 0, is_doubl = 0;
	char single[2], doubl[3];
	single[1] = 0;
	doubl[2] = 0;

	single[0] = token;
	doubl[0] = token;

	//if end of file put in garbage for doubl
	if (get_token()) {
		doubl[1] = ']';
	} else {
		doubl[1] = token;
	}

	for (i = 0; i < NUM_SYMBOLS; i++) {
		if (strcmp(symbols[i], single) == 0) {
			is_single = i + 1;
		}
		if (strcmp(symbols[i], doubl) == 0) {
			is_doubl = i + 1;
		}
	}

	if (!is_single && !is_doubl) {
		printf("Error: Unknown Symbol [%s] at %d:%d\n", single, prev_line, prev_col);
		return -1;
	}

	if (is_doubl) {
		strcpy(ident_buffer, doubl);
		add_to_lexeme(1, is_doubl - 1);
		get_token();
	} else {
		strcpy(ident_buffer, single);
		add_to_lexeme(1, is_single - 1);
	}

	return 0;
}

void print_code() {
	int i;

	printf("Source Program:\n");

	for (i = 0; i < code_length; i++) {
		printf("%c", code[i]);
	}

	printf("\n\n");
}

void print_lexeme_list() {
	TOKEN *tok = start;

	printf("Lexeme List:\n");

	while (tok != NULL) {
		if (tok->type == 2 || tok->type == 3)
			printf("%d %s ", tok->type, tok->value);
		else
			printf("%d ", tok->type);
		tok = tok->next;
	}

	printf("\n");
}

void print_lexeme_table() {
	TOKEN *tok = start;

	printf("Lexeme Table:\nLexeme\t\tToken Type\n");

	while (tok != NULL) {
		printf("%s\t\t%d\n", tok->value, tok->type);
		tok = tok->next;
	}

	printf("\n");
}

void remove_comments() {
	int type = 0;

	ident_buffer[0] = token;

	if (get_token() == -1) {
		add_to_lexeme(1, 7);
		return;
	}

	if (token != '/' && token != '*') {
		add_to_lexeme(1, 7); // add slash to lexeme not part of comments
		return;
	}

	if (token == '/')
		type = 1;
	if (token == '*')
		type = 2;

	while (1) {
		if (get_token())
			return;

		if (type == 1 && token == '\n')
			break;
		if (type == 2 && token == '*') {
			if (get_token())
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
	prev_col = col;
	prev_line = line;
	if (token == '\n') {
		line++;
		col = 0;
	} else if (token == '\t') {
		col += 5;
	} else {
		col++;
	}

	if (code_length > code_alloc) {
		code_alloc = code_length + 500;
		code = realloc(code, code_alloc * sizeof(int));
	}
	code[code_length++] = token;
	return 0;
}

int number() {
	int length = 1;

	ident_buffer[0] = token;

	//if end of file just assume what we have is an number
	if (get_token() == -1)
		goto NUMBER_EXIT;

	 while (length < NUM_MAX_LENGTH) {
		 if (isdigit(token)) {
			ident_buffer[length++] = token;
			if (get_token())
				goto NUMBER_EXIT;
			continue;
		}
		 break;
	}

	 if (isalpha(token)) {
		 printf("Error: Identifier cannot start with a number! %d:%d", line, col);
		 return -1;
	 }

	 if (length == NUM_MAX_LENGTH) {
		 if (isdigit(token)) {
			 printf("Error: Number [%s] at %d:%d exceeds max size...\n", ident_buffer, prev_line, (int)(prev_col- strlen(ident_buffer)));
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
			if (get_token())
				goto IDENT_RES_EXIT;
			continue;
		}
		break;
	}

	if (length == IDENT_MAX_LENGTH) {
		if (isalpha(token)) {
			printf("Error: Identifier [%s] at %d:%d exceeds max size...\n", ident_buffer, prev_line, (int)(prev_col - strlen(ident_buffer)));
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
//type: 3 = number lex = Anyvalue
void add_to_lexeme(int type, int lex) {
	TOKEN *tok;

	tok = malloc(sizeof(TOKEN));
	tok->next = NULL;

	if (type == 0) {
		tok->type = reserved_lex[lex];
	} else if (type == 1) {
		tok->type = symbol_lex[lex];
	} else if (type == 2) {
		tok->type = ident_sym;
	} else if (type == 3) {
		tok->type = num_sym;
	}

	strcpy(tok->value, ident_buffer);

	if (start == NULL) {
		start = tok;
		end = tok;
	} else {
		end->next = tok;
		end = tok;
	}
}

void free_tokens() {
	TOKEN *temp;
	TOKEN *next;

	if(start == NULL)
		return;

	if(start->next == NULL) {
		free(start);
		return;
	}

	temp = start;
	next = start->next;

	while(next != NULL) {
		free(temp);
		temp = next;
		next = next->next;
	}

	free(temp);
}
