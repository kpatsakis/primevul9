static int fcn_list_json(RCore *core, RList *fcns, bool quiet) {
	RListIter *iter;
	RAnalFunction *fcn;
	PJ *pj = r_core_pj_new (core);
	if (!pj) {
		r_cons_println ("[]");
		return -1;
	}
	pj_a (pj);
	r_list_foreach (fcns, iter, fcn) {
		if (quiet) {
			pj_n (pj, fcn->addr);
		} else {
			fcn_print_json (core, fcn, pj);
		}
	}
	pj_end (pj);
	r_cons_println (pj_string (pj));
	pj_free (pj);
	return 0;
}