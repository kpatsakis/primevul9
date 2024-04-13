static inline int is_var(char *x) {
	return x[0] == '.' || ((x[0] == '*' || x[0] == '&') && x[1] == '.');
}