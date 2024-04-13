static void autoname_imp_trampoline(RCore *core, RAnalFunction *fcn) {
	if (r_list_length (fcn->bbs) == 1 && ((RAnalBlock *) r_list_first (fcn->bbs))->ninstr == 1) {
		RList *refs = r_anal_function_get_refs (fcn);
		if (refs && r_list_length (refs) == 1) {
			RAnalRef *ref = r_list_first (refs);
			if (ref->type != R_ANAL_REF_TYPE_CALL) { /* Some fcns don't return */
				RFlagItem *flg = r_flag_get_i (core->flags, ref->addr);
				if (flg && r_str_startswith (flg->name, "sym.imp.")) {
					R_FREE (fcn->name);
					fcn->name = r_str_newf ("sub.%s", flg->name + 8);
				}
			}
		}
		r_list_free (refs);
	}
}