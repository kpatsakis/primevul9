static int fcn_print_detail(RCore *core, RAnalFunction *fcn) {
	const char *defaultCC = r_anal_cc_default (core->anal);
	char *name = r_core_anal_fcn_name (core, fcn);
	char *paren = strchr (name, '(');
	if (paren) {
		*paren = '\0';
	}
	r_cons_printf ("\"f %s %"PFMT64u" 0x%08"PFMT64x"\"\n", name, r_anal_function_linear_size (fcn), fcn->addr);
	r_cons_printf ("\"af+ 0x%08"PFMT64x" %s %c %c\"\n",
			fcn->addr, name, //r_anal_function_size (fcn), name,
			fcn->type == R_ANAL_FCN_TYPE_LOC?'l':
			fcn->type == R_ANAL_FCN_TYPE_SYM?'s':
			fcn->type == R_ANAL_FCN_TYPE_IMP?'i':'f',
			fcn->diff->type == R_ANAL_DIFF_TYPE_MATCH?'m':
			fcn->diff->type == R_ANAL_DIFF_TYPE_UNMATCH?'u':'n');
	// FIXME: this command prints something annoying. Does it have important side-effects?
	fcn_list_bbs (fcn);
	if (fcn->bits != 0) {
		r_cons_printf ("afB %d @ 0x%08"PFMT64x"\n", fcn->bits, fcn->addr);
	}
	// FIXME command injection vuln here
	if (fcn->cc || defaultCC) {
		r_cons_printf ("s 0x%"PFMT64x"\n", fcn->addr);
		r_cons_printf ("\"afc %s\"\n", fcn->cc? fcn->cc: defaultCC);
		r_cons_println ("s-");
	}
	if (fcn->folded) {
		r_cons_printf ("afF @ 0x%08"PFMT64x"\n", fcn->addr);
	}
	if (fcn) {
		/* show variables  and arguments */
		r_core_cmdf (core, "afvb* @ 0x%"PFMT64x"\n", fcn->addr);
		r_core_cmdf (core, "afvr* @ 0x%"PFMT64x"\n", fcn->addr);
		r_core_cmdf (core, "afvs* @ 0x%"PFMT64x"\n", fcn->addr);
	}
	/* Show references */
	RListIter *refiter;
	RAnalRef *refi;
	RList *refs = r_anal_function_get_refs (fcn);
	r_list_foreach (refs, refiter, refi) {
		switch (refi->type) {
		case R_ANAL_REF_TYPE_CALL:
			r_cons_printf ("axC 0x%"PFMT64x" 0x%"PFMT64x"\n", refi->addr, refi->at);
			break;
		case R_ANAL_REF_TYPE_DATA:
			r_cons_printf ("axd 0x%"PFMT64x" 0x%"PFMT64x"\n", refi->addr, refi->at);
			break;
		case R_ANAL_REF_TYPE_CODE:
			r_cons_printf ("axc 0x%"PFMT64x" 0x%"PFMT64x"\n", refi->addr, refi->at);
			break;
		case R_ANAL_REF_TYPE_STRING:
			r_cons_printf ("axs 0x%"PFMT64x" 0x%"PFMT64x"\n", refi->addr, refi->at);
			break;
		case R_ANAL_REF_TYPE_NULL:
		default:
			r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x"\n", refi->addr, refi->at);
			break;
		}
	}
	r_list_free (refs);
	/*Saving Function stack frame*/
	r_cons_printf ("afS %d @ 0x%"PFMT64x"\n", fcn->maxstack, fcn->addr);
	free (name);
	return 0;
}