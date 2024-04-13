static int cmpname(const void *_a, const void *_b) {
	const RAnalFunction *a = _a, *b = _b;
	int as = strcmp (a->name, b->name);
	int bs = strcmp (b->name, a->name);
	return (as > bs)? 1: (as < bs)? -1: 0;
}