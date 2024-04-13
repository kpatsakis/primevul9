static char *anal_fcn_autoname(RCore *core, RAnalFunction *fcn, int dump, int mode) {
	int use_getopt = 0;
	int use_isatty = 0;
	PJ *pj = NULL;
	char *do_call = NULL;
	RAnalRef *ref;
	RListIter *iter;
	RList *refs = r_anal_function_get_refs (fcn);
	if (mode == 'j') {
		// start a new JSON object
		pj = r_core_pj_new (core);
		pj_a (pj);
	}
	if (refs) {
		r_list_foreach (refs, iter, ref) {
			RFlagItem *f = r_flag_get_i (core->flags, ref->addr);
			if (f) {
				// If dump is true, print all strings referenced by the function
				if (dump) {
					// take only strings flags
					if (!strncmp (f->name, "str.", 4)) {
						if (mode == 'j') {
							// add new json item
							pj_o (pj);
							pj_kn (pj, "addr", ref->at);
							pj_kn (pj, "ref", ref->addr);
							pj_ks (pj, "flag", f->name);
							pj_end (pj);
						} else {
							r_cons_printf ("0x%08"PFMT64x" 0x%08"PFMT64x" %s\n", ref->at, ref->addr, f->name);
						}
					}
				} else if (do_call) { // break if a proper autoname found and not in dump mode
					break;
				}
				// enter only if a candidate name hasn't found yet
				if (!do_call) {
					if (blacklisted_word (f->name)) {
						continue;
					}
					if (strstr (f->name, ".isatty")) {
						use_isatty = 1;
					}
					if (strstr (f->name, ".getopt")) {
						use_getopt = 1;
					}
					if (!strncmp (f->name, "method.", 7)) {
						free (do_call);
						do_call = strdup (f->name + 7);
						continue;
					}
					if (!strncmp (f->name, "str.", 4)) {
						free (do_call);
						do_call = strdup (f->name + 4);
						continue;
					}
					if (!strncmp (f->name, "dbg.", 4)) {
						free (do_call);
						do_call = strdup (f->name + 4);
						continue;
					}
					if (!strncmp (f->name, "sym.imp.", 8)) {
						free (do_call);
						do_call = strdup (f->name + 8);
						continue;
					}
					if (!strncmp (f->name, "reloc.", 6)) {
						free (do_call);
						do_call = strdup (f->name + 6);
						continue;
					}
				}
			}
		}
		r_list_free (refs);
	}
	if (mode ==  'j') {
		pj_end (pj);
	}
	if (pj) {
		r_cons_printf ("%s\n", pj_string (pj));
		pj_free (pj);
	}
	// TODO: append counter if name already exists
	if (use_getopt) {
		RFlagItem *item = r_flag_get (core->flags, "main");
		free (do_call);
		// if referenced from entrypoint. this should be main
		if (item && item->offset == fcn->addr) {
			return strdup ("main"); // main?
		}
		return strdup ("parse_args"); // main?
	}
	if (use_isatty) {
		char *ret = r_str_newf ("sub.setup_tty_%s_%"PFMT64x, do_call, fcn->addr);
		free (do_call);
		return ret;
	}
	if (do_call) {
		char *ret = r_str_newf ("sub.%s_%"PFMT64x, do_call, fcn->addr);
		free (do_call);
		return ret;
	}
	return NULL;
}