static int fcn_print_legacy(RCore *core, RAnalFunction *fcn) {
	RListIter *iter;
	RAnalRef *refi;
	RList *refs, *xrefs;
	int ebbs = 0;
	char *name = r_core_anal_fcn_name (core, fcn);

	r_cons_printf ("#\noffset: 0x%08"PFMT64x"\nname: %s\nsize: %"PFMT64u,
			fcn->addr, name, r_anal_function_linear_size (fcn));
	r_cons_printf ("\nis-pure: %s", r_str_bool (r_anal_function_purity (fcn)));
	r_cons_printf ("\nrealsz: %" PFMT64d, r_anal_function_realsize (fcn));
	r_cons_printf ("\nstackframe: %d", fcn->maxstack);
	if (fcn->cc) {
		r_cons_printf ("\ncall-convention: %s", fcn->cc);
	}
	r_cons_printf ("\ncyclomatic-cost: %d", r_anal_function_cost (fcn));
	r_cons_printf ("\ncyclomatic-complexity: %d", r_anal_function_complexity (fcn));
	r_cons_printf ("\nbits: %d", fcn->bits);
	r_cons_printf ("\ntype: %s", r_anal_functiontype_tostring (fcn->type));
	if (fcn->type == R_ANAL_FCN_TYPE_FCN || fcn->type == R_ANAL_FCN_TYPE_SYM) {
		r_cons_printf (" [%s]",
				fcn->diff->type == R_ANAL_DIFF_TYPE_MATCH?"MATCH":
				fcn->diff->type == R_ANAL_DIFF_TYPE_UNMATCH?"UNMATCH":"NEW");
	}
	r_cons_printf ("\nnum-bbs: %d", r_list_length (fcn->bbs));
	r_cons_printf ("\nedges: %d", r_anal_function_count_edges (fcn, &ebbs));
	r_cons_printf ("\nend-bbs: %d", ebbs);
	r_cons_printf ("\ncall-refs:");
	int outdegree = 0;
	refs = r_anal_function_get_refs (fcn);
	r_list_foreach (refs, iter, refi) {
		if (refi->type == R_ANAL_REF_TYPE_CALL) {
			outdegree++;
		}
		if (refi->type == R_ANAL_REF_TYPE_CODE || refi->type == R_ANAL_REF_TYPE_CALL) {
			r_cons_printf (" 0x%08"PFMT64x" %c", refi->addr,
					refi->type == R_ANAL_REF_TYPE_CALL?'C':'J');
		}
	}
	r_cons_printf ("\ndata-refs:");
	r_list_foreach (refs, iter, refi) {
		// global or local?
		if (refi->type == R_ANAL_REF_TYPE_DATA) {
			r_cons_printf (" 0x%08"PFMT64x, refi->addr);
		}
	}
	r_list_free (refs);

	int indegree = 0;
	r_cons_printf ("\ncode-xrefs:");
	xrefs = r_anal_function_get_xrefs (fcn);
	r_list_foreach (xrefs, iter, refi) {
		if (refi->type == R_ANAL_REF_TYPE_CODE || refi->type == R_ANAL_REF_TYPE_CALL) {
			indegree++;
			r_cons_printf (" 0x%08"PFMT64x" %c", refi->addr,
					refi->type == R_ANAL_REF_TYPE_CALL?'C':'J');
		}
	}
	r_cons_printf ("\nnoreturn: %s", r_str_bool (fcn->is_noreturn));
	r_cons_printf ("\nin-degree: %d", indegree);
	r_cons_printf ("\nout-degree: %d", outdegree);
	r_cons_printf ("\ndata-xrefs:");
	r_list_foreach (xrefs, iter, refi) {
		if (refi->type == R_ANAL_REF_TYPE_DATA) {
			r_cons_printf (" 0x%08"PFMT64x, refi->addr);
		}
	}
	r_list_free (xrefs);

	if (fcn->type == R_ANAL_FCN_TYPE_FCN || fcn->type == R_ANAL_FCN_TYPE_SYM) {
		int args_count = r_anal_var_count_args (fcn);
		int var_count = r_anal_var_count_locals (fcn);

		r_cons_printf ("\nlocals: %d\nargs: %d\n", var_count, args_count);
		r_anal_var_list_show (core->anal, fcn, 'b', 0, NULL);
		r_anal_var_list_show (core->anal, fcn, 's', 0, NULL);
		r_anal_var_list_show (core->anal, fcn, 'r', 0, NULL);
		r_cons_printf ("diff: type: %s",
				fcn->diff->type == R_ANAL_DIFF_TYPE_MATCH?"match":
				fcn->diff->type == R_ANAL_DIFF_TYPE_UNMATCH?"unmatch":"new");
		if (fcn->diff->addr != -1) {
			r_cons_printf ("addr: 0x%"PFMT64x, fcn->diff->addr);
		}
		if (fcn->diff->name) {
			r_cons_printf ("function: %s", fcn->diff->name);
		}
	}
	free (name);

	// traced
	if (core->dbg->trace->enabled) {
		is_fcn_traced (core->dbg->trace, fcn);
	}
	return 0;
}