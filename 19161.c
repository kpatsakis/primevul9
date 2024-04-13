R_API bool r_core_anal_bb_seek(RCore *core, ut64 addr) {
	ut64 bbaddr = r_anal_get_bbaddr (core->anal, addr);
	if (bbaddr != UT64_MAX) {
		r_core_seek (core, bbaddr, false);
		return true;
	}
	return false;
}