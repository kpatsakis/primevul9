R_API int r_core_anal_fcn_clean(RCore *core, ut64 addr) {
	RAnalFunction *fcni;
	RListIter *iter, *iter_tmp;

	if (!addr) {
		r_list_purge (core->anal->fcns);
		if (!(core->anal->fcns = r_list_new ())) {
			return false;
		}
	} else {
		r_list_foreach_safe (core->anal->fcns, iter, iter_tmp, fcni) {
			if (r_anal_function_contains (fcni, addr)) {
				r_anal_function_delete (fcni);
			}
		}
	}
	return true;
}