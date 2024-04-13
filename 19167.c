static int fcn_list_legacy(RCore *core, RList *fcns) {
	RListIter *iter;
	RAnalFunction *fcn;
	r_list_foreach (fcns, iter, fcn) {
		fcn_print_legacy (core, fcn);
	}
	r_cons_newline ();
	return 0;
}