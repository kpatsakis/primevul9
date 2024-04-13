static int fcn_print_json(RCore *core, RAnalFunction *fcn, PJ *pj) {
	RListIter *iter;
	RAnalRef *refi;
	RList *refs, *xrefs;
	if (!pj) {
		return -1;
	}
	int ebbs = 0;
	pj_o (pj);
	pj_kn (pj, "offset", fcn->addr);
	char *name = r_core_anal_fcn_name (core, fcn);
	if (name) {
		pj_ks (pj, "name", name);
	}
	pj_kn (pj, "size", r_anal_function_linear_size (fcn));
	pj_ks (pj, "is-pure", r_str_bool (r_anal_function_purity (fcn)));
	pj_kn (pj, "realsz", r_anal_function_realsize (fcn));
	pj_kb (pj, "noreturn", fcn->is_noreturn);
	pj_ki (pj, "stackframe", fcn->maxstack);
	if (fcn->cc) {
		pj_ks (pj, "calltype", fcn->cc); // calling conventions
	}
	pj_ki (pj, "cost", r_anal_function_cost (fcn)); // execution cost
	pj_ki (pj, "cc", r_anal_function_complexity (fcn)); // cyclic cost
	pj_ki (pj, "bits", fcn->bits);
	pj_ks (pj, "type", r_anal_functiontype_tostring (fcn->type));
	pj_ki (pj, "nbbs", r_list_length (fcn->bbs));
	pj_ki (pj, "edges", r_anal_function_count_edges (fcn, &ebbs));
	pj_ki (pj, "ebbs", ebbs);
	{
		char *sig = r_core_cmd_strf (core, "afcf @ 0x%"PFMT64x, fcn->addr);
		if (sig) {
			r_str_trim (sig);
			pj_ks (pj, "signature", sig);
			free (sig);
		}

	}
	pj_kn (pj, "minbound", r_anal_function_min_addr (fcn));
	pj_kn (pj, "maxbound", r_anal_function_max_addr (fcn));

	int outdegree = 0;
	refs = r_anal_function_get_refs (fcn);
	if (!r_list_empty (refs)) {
		pj_k (pj, "callrefs");
		pj_a (pj);
		r_list_foreach (refs, iter, refi) {
			if (refi->type == R_ANAL_REF_TYPE_CALL) {
				outdegree++;
			}
			if (refi->type == R_ANAL_REF_TYPE_CODE ||
				refi->type == R_ANAL_REF_TYPE_CALL) {
				pj_o (pj);
				pj_kn (pj, "addr", refi->addr);
				pj_ks (pj, "type", r_anal_xrefs_type_tostring (refi->type));
				pj_kn (pj, "at", refi->at);
				pj_end (pj);
			}
		}
		pj_end (pj);

		pj_k (pj, "datarefs");
		pj_a (pj);
		r_list_foreach (refs, iter, refi) {
			if (refi->type == R_ANAL_REF_TYPE_DATA) {
				pj_n (pj, refi->addr);
			}
		}
		pj_end (pj);
	}
	r_list_free (refs);

	int indegree = 0;
	xrefs = r_anal_function_get_xrefs (fcn);
	if (!r_list_empty (xrefs)) {
		pj_k (pj, "codexrefs");
		pj_a (pj);
		r_list_foreach (xrefs, iter, refi) {
			if (refi->type == R_ANAL_REF_TYPE_CODE ||
				refi->type == R_ANAL_REF_TYPE_CALL) {
				indegree++;
				pj_o (pj);
				pj_kn (pj, "addr", refi->addr);
				pj_ks (pj, "type", r_anal_xrefs_type_tostring (refi->type));
				pj_kn (pj, "at", refi->at);
				pj_end (pj);
			}
		}

		pj_end (pj);
		pj_k (pj, "dataxrefs");
		pj_a (pj);

		r_list_foreach (xrefs, iter, refi) {
			if (refi->type == R_ANAL_REF_TYPE_DATA) {
				pj_n (pj, refi->addr);
			}
		}
		pj_end (pj);
	}
	r_list_free (xrefs);

	pj_ki (pj, "indegree", indegree);
	pj_ki (pj, "outdegree", outdegree);

	if (fcn->type == R_ANAL_FCN_TYPE_FCN || fcn->type == R_ANAL_FCN_TYPE_SYM) {
		pj_ki (pj, "nlocals", r_anal_var_count_locals (fcn));
		pj_ki (pj, "nargs", r_anal_var_count_args (fcn));
		pj_k (pj, "bpvars");
		r_anal_var_list_show (core->anal, fcn, 'b', 'j', pj);
		pj_k (pj, "spvars");
		r_anal_var_list_show (core->anal, fcn, 's', 'j', pj);
		pj_k (pj, "regvars");
		r_anal_var_list_show (core->anal, fcn, 'r', 'j', pj);

		pj_ks (pj, "difftype", fcn->diff->type == R_ANAL_DIFF_TYPE_MATCH?"match":
				fcn->diff->type == R_ANAL_DIFF_TYPE_UNMATCH?"unmatch":"new");
		if (fcn->diff->addr != -1) {
			pj_kn (pj, "diffaddr", fcn->diff->addr);
		}
		if (fcn->diff->name) {
			pj_ks (pj, "diffname", fcn->diff->name);
		}
	}
	pj_end (pj);
	free (name);
	return 0;
}