static int cmpsize(const void *a, const void *b) {
	ut64 as = r_anal_function_linear_size ((RAnalFunction *) a);
	ut64 bs = r_anal_function_linear_size ((RAnalFunction *) b);
	return (as> bs)? 1: (as< bs)? -1: 0;
}