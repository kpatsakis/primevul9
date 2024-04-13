static int cmpframe(const void *_a, const void *_b) {
	const RAnalFunction *a = _a, *b = _b;
	int as = a->maxstack;
	int bs = b->maxstack;
	return (as > bs)? 1: (as < bs)? -1: 0;
}