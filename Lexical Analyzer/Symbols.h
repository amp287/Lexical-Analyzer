#define IDENT_MAX_LENGTH 10
#define NUM_SYMBOLS 14

enum lex {
	nul_sym = 1, ident_sym, num_sym, plus_sym, minus_sym, mult_sym, slash_sym, odd_sym, eql_sym, neq_sym, less_sym, leq_sym, gtr_sym,
	geq_sym, lparent_sym, rparent_sym, comma_sym, semicol_sym, period_sym, becomes_sym, begin_sym, end_sym, if_sym, then_sym, while_sym,
	do_sym, call_sym, const_sym, var_sym, proc_sym, write_sym, read_sym, else_sym
};

char *symbols = "+-*/()=,.<>;:";
char *reserved[] = { "const","var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write" };
int reserved_lex[] = { const_sym, var_sym, proc_sym, call_sym, begin_sym, end_sym, if_sym, then_sym, else};

