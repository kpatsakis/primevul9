static int cmpbbs (const void *_a, const void *_b) {
	const RAnalFunction *a = _a, *b = _b;
	int la = (int)r_list_length (a->bbs);
	int lb = (int)r_list_length (b->bbs);
	return (la > lb)? -1: (la < lb)? 1 : 0;
}