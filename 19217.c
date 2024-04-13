R_API ut64 r_core_anal_fcn_list_size(RCore *core) {
	RAnalFunction *fcn;
	RListIter *iter;
	ut64 total = 0;

	r_list_foreach (core->anal->fcns, iter, fcn) {
		total += r_anal_function_realsize (fcn);
	}
	r_cons_printf ("%"PFMT64u"\n", total);
	return total;
}