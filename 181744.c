static int delta_cmp(const void *a, const void *b) {
	const RAnalVar *va = a;
	const RAnalVar *vb = b;
	return vb->delta - va->delta;
}