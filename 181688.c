static int mymemread(RAnalEsil *esil, ut64 addr, ut8 *buf, int len) {
	RListIter *iter;
	AeaMemItem *n;
	r_list_foreach (mymemxsr, iter, n) {
		if (addr == n->addr) {
			return len;
		}
	}
	if (!r_io_is_valid_offset (esil->anal->iob.io, addr, 0)) {
		return false;
	}
	n = R_NEW (AeaMemItem);
	if (n) {
		n->addr = addr;
		n->size = len;
		r_list_push (mymemxsr, n);
	}
	return len;
}