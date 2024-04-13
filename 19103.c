static int cmpfcncc(const void *_a, const void *_b) {
	RAnalFunction *a = (RAnalFunction *)_a;
	RAnalFunction *b = (RAnalFunction *)_b;
	ut64 as = r_anal_function_complexity (a);
	ut64 bs = r_anal_function_complexity (b);
	return (as > bs)? 1: (as < bs)? -1: 0;
}