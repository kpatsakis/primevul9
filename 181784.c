static int delta_cmp2(const void *a, const void *b) {
	const RAnalVar *va = a;
	const RAnalVar *vb = b;
	return va->delta - vb->delta;
}