#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

typedef struct {
	int type;
	char value[10];

}TOKEN;

int var_key_state;
char *code;
int code_length;

int variable_or_keyword(char *input, int length, int state);
int read_file(FILE *fp);
void remove_comments();
void print_code();

int main(int argc, char *argv[]) {
	FILE *fp;

	/*if (argc < 2) {
		printf("ERROR: PL/0 file must be provided in arguments!");
		return -1;
	}*/

	//fp = fopen(argv[1], "r");

	//read_file(argv[1], strlen(argv[1]));
	fp = fopen(argv[1], "r");

	read_file(fp);

	remove_comments();

	print_code();

return 0;
}

void print_code(){
	int i;
	for(i = 0; i < code_length; i++) {
		printf("%c", code[i]);
	}
}

void remove_comments(){
	char buffer;
	int in_comments = 0;
	int count = 0;

	while(count != code_length){

		if(buffer == '/'){
			if(in_comments == 1){
				in_comments == 2;
				continue;
			}
			in_comments = 1;
		}

		if(in_comments == 2){
			if(buffer == '\n')
				in_comments = 0;
			else
				continue;
		}
		printf("%c", buffer);
		if(count >= code_length){
			input = realloc(input, code_length + 500);
			code_length += 500;
		}
		input[count] = buffer;
		count++;
	}
	code_length = count;
}

int read_file(FILE *fp) {
	char buffer;
	int count = 0;

	input = calloc(500, sizeof(char));
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

int variable_or_keyword(char *input, int length, int state) {
	int ret = 0;

	if(length == 0)
		return 1;

	printf("input:%c, length:%d, State:%d\n", *input, length, state);

	if(state == 1) {
		if(isalpha(*input)) {
			return variable_or_keyword(++input, --length, 2);
		}
		return 0;
	} else if(state == 2) {
		if(isalpha(*input) || isdigit(*input)){
			return variable_or_keyword(++input, --length, 2);
		}
		return 0;
	}

	return 0;
}
