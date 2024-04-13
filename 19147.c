static int cmpcalls(const void *_a, const void *_b) {
	const RAnalFunction *a = _a, *b = _b;
	int as = a->meta.numcallrefs;
	int bs = b->meta.numcallrefs;
	return (as > bs)? 1: (as < bs)? -1: 0;
}