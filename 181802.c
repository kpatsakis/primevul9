static int cmpsize (const void *a, const void *b) {
	ut64 sa = (int) r_anal_function_linear_size ((RAnalFunction *) a);
	ut64 sb = (int) r_anal_function_linear_size ((RAnalFunction *) b);
	return (sa > sb)? -1: (sa < sb)? 1 : 0;
}