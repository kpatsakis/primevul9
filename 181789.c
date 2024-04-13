static bool anal_fcn_del_bb(RCore *core, const char *input) {
	ut64 addr = r_num_math (core->num, input);
	if (!addr) {
		addr = core->offset;
	}
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, -1);
	if (fcn) {
		if (!strcmp (input, "*")) {
			while (!r_list_empty (fcn->bbs)) {
				r_anal_function_remove_block (fcn, r_list_first (fcn->bbs));
			}
		} else {
			RAnalBlock *b;
			RListIter *iter;
			r_list_foreach (fcn->bbs, iter, b) {
				if (b->addr == addr) {
					r_anal_function_remove_block (fcn, b);
					return true;
				}
			}
			eprintf ("Cannot find basic block\n");
		}
	} else {
		eprintf ("Cannot find function\n");
	}
	return false;
}