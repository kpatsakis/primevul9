static int find_bb(ut64 *addr, RAnalBlock *bb) {
	return *addr != bb->addr;
}