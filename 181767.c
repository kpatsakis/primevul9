static int compute_calls(RCore *core) {
	RListIter *iter;
	RAnalFunction *fcn;
	RList *xrefs;
	int cov = 0;
	r_list_foreach (core->anal->fcns, iter, fcn) {
		xrefs = r_anal_fcn_get_xrefs (core->anal, fcn);
		if (xrefs) {
			cov += r_list_length (xrefs);
			r_list_free (xrefs);
			xrefs = NULL;
		}
	}
	return cov;
}