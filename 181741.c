static void r_core_anal_info (RCore *core, const char *input) {
	int fcns = r_list_length (core->anal->fcns);
	int strs = r_flag_count (core->flags, "str.*");
	int syms = r_flag_count (core->flags, "sym.*");
	int imps = r_flag_count (core->flags, "sym.imp.*");
	int code = compute_code (core);
	int covr = compute_coverage (core);
	int call = compute_calls (core);
	int xrfs = r_anal_xrefs_count (core->anal);
	int cvpc = (code > 0)? (covr * 100 / code): 0;
	if (*input == 'j') {
		PJ *pj = pj_new ();
		if (!pj) {
			return;
		}
		pj_o (pj);
		pj_ki (pj, "fcns", fcns);
		pj_ki (pj, "xrefs", xrfs);
		pj_ki (pj, "calls", call);
		pj_ki (pj, "strings", strs);
		pj_ki (pj, "symbols", syms);
		pj_ki (pj, "imports", imps);
		pj_ki (pj, "covrage", covr);
		pj_ki (pj, "codesz", code);
		pj_ki (pj, "percent", cvpc);
		pj_end (pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
	} else {
		r_cons_printf ("fcns    %d\n", fcns);
		r_cons_printf ("xrefs   %d\n", xrfs);
		r_cons_printf ("calls   %d\n", call);
		r_cons_printf ("strings %d\n", strs);
		r_cons_printf ("symbols %d\n", syms);
		r_cons_printf ("imports %d\n", imps);
		r_cons_printf ("covrage %d\n", covr);
		r_cons_printf ("codesz  %d\n", code);
		r_cons_printf ("percent %d%%\n", cvpc);
	}
}