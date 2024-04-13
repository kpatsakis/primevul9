static inline int is_op(char x) {
	switch (x) {
	case '-':
	case '+':
		return 1;
	case '*':
	case '/':
		return 2;
	case '^':
	case '|':
	case '&':
		return 3;
	default:
		return 0;
	}
}