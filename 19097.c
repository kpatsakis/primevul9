static bool is_skippable_addr(RCore *core, ut64 addr) {
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, 0);
	if (!fcn) {
		return false;
	}
	if (fcn->addr == addr) {
		return true;
	}
	const RList *flags = r_flag_get_list (core->flags, addr);
	return !(flags && r_list_find (flags, fcn, find_sym_flag));
}