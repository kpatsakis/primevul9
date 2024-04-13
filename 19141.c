static void analPathFollow(RCoreAnalPaths *p, ut64 addr, PJ *pj) {
	if (addr == UT64_MAX) {
		return;
	}
	if (!dict_get (&p->visited, addr)) {
		p->cur = r_anal_bb_from_offset (p->core->anal, addr);
		analPaths (p, pj);
	}
}