static int casecmp(const void* _a, const void * _b) {
	const RAnalCaseOp* a = _a;
	const RAnalCaseOp* b = _b;
	return a->addr != b->addr;
}