static int cmpedges(const void *_a, const void *_b) {
	const RAnalFunction *a = _a, *b = _b;
	int as, bs;
	r_anal_function_count_edges (a, &as);
	r_anal_function_count_edges (b, &bs);
	return (as > bs)? 1: (as < bs)? -1: 0;
}