static int RAnalRef_cmp(const RAnalRef* ref1, const RAnalRef* ref2) {
	return ref1->addr != ref2->addr;
}