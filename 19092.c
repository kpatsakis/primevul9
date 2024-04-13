static void __fcn_print_default(RCore *core, RAnalFunction *fcn, bool quiet) {
	if (quiet) {
		r_cons_printf ("0x%08"PFMT64x" ", fcn->addr);
	} else {
		char *name = r_core_anal_fcn_name (core, fcn);
		ut64 realsize = r_anal_function_realsize (fcn);
		ut64 size = r_anal_function_linear_size (fcn);
		char *msg = (realsize == size)
			? r_str_newf ("%-12"PFMT64u, size)
			: r_str_newf ("%-4"PFMT64u" -> %-4"PFMT64u, size, realsize);
		r_cons_printf ("0x%08"PFMT64x" %4d %4s %s\n",
				fcn->addr, r_list_length (fcn->bbs), msg, name);
		free (name);
		free (msg);
	}
}