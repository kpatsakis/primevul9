static void var_accesses_list(RAnal *a, RAnalFunction *fcn, int delta, const char *typestr) {
	const char *var_local = sdb_fmt ("var.0x%"PFMT64x".%d.%d.%s",
			fcn->addr, 1, delta, typestr);
	const char *xss = sdb_const_get (a->sdb_fcns, var_local, 0);
	if (xss && *xss) {
		r_cons_printf ("%s\n", xss);
	} else {
		r_cons_newline ();
	}
}