static int bb_cmp(const void *a, const void *b) {
	const RAnalBlock *ba = a;
	const RAnalBlock *bb = b;
	return ba->addr - bb->addr;
}