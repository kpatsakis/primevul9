static int fcn_list_default(RCore *core, RList *fcns, bool quiet) {
	RListIter *iter;
	RAnalFunction *fcn;
	r_list_foreach (fcns, iter, fcn) {
		__fcn_print_default (core, fcn, quiet);
		if (quiet) {
			r_cons_newline ();
		}
	}
	return 0;
}