static int fcn_list_table(RCore *core, const char *q, int fmt) {
	char xref[128], ccstr[128], castr[128];
	RAnalFunction *fcn;
	RListIter *iter;
	RTable *t = r_core_table (core, "fcns");
	RTableColumnType *typeString = r_table_type ("string");
	RTableColumnType *typeNumber = r_table_type ("number");
	r_table_add_column (t, typeNumber, "addr", 0);
	r_table_add_column (t, typeNumber, "size", 0);
	r_table_add_column (t, typeString, "name", 0);
	r_table_add_column (t, typeNumber, "nbbs", 0);
	r_table_add_column (t, typeNumber, "xref", 0);
	r_table_add_column (t, typeNumber, "calls", 0);
	r_table_add_column (t, typeNumber, "cc", 0);
	r_list_foreach (core->anal->fcns, iter, fcn) {
		r_strf_var (fcnAddr, 32, "0x%08"PFMT64x, fcn->addr);
		r_strf_var (fcnSize, 32, "%"PFMT64u, r_anal_function_linear_size (fcn)); // r_anal_function_size (fcn));
		r_strf_var (nbbs, 32, "%d", r_list_length (fcn->bbs));
		RList *xrefs = r_anal_function_get_xrefs (fcn);
		snprintf (xref, sizeof (xref), "%d", r_list_length (xrefs));
		r_list_free (xrefs);

		RList *calls = r_core_anal_fcn_get_calls (core, fcn);
		r_list_uniq_inplace (calls, (RListComparatorItem)RAnalRef_val);
		snprintf (castr, sizeof (castr), "%d", r_list_length (calls));
		r_list_free (calls);
		snprintf (ccstr, sizeof (ccstr), "%d", r_anal_function_complexity (fcn));

		r_table_add_row (t, fcnAddr, fcnSize, fcn->name, nbbs, xref, castr, ccstr, NULL);
	}
	if (r_table_query (t, q)) {
		char *s = (fmt == 'j')
			? r_table_tojson (t)
			: r_table_tostring (t);
		r_cons_printf ("%s\n", s);
		free (s);
	}
	r_table_free (t);
	return 0;
}