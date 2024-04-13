static int cmpnbbs(const void *_a, const void *_b) {
	const RAnalFunction *a = _a, *b = _b;
	ut64 as = r_list_length (a->bbs);
	ut64 bs = r_list_length (b->bbs);
	return (as> bs)? 1: (as< bs)? -1: 0;
}