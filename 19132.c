R_API void r_core_anal_fcn_merge(RCore *core, ut64 addr, ut64 addr2) {
	RListIter *iter;
	ut64 min = 0;
	ut64 max = 0;
	int first = 1;
	RAnalBlock *bb;
	RAnalFunction *f1 = r_anal_get_function_at (core->anal, addr);
	RAnalFunction *f2 = r_anal_get_function_at (core->anal, addr2);
	if (!f1 || !f2) {
		eprintf ("Cannot find function\n");
		return;
	}
	if (f1 == f2) {
		eprintf ("Cannot merge the same function\n");
		return;
	}
	// join all basic blocks from f1 into f2 if they are not
	// delete f2
	eprintf ("Merge 0x%08"PFMT64x" into 0x%08"PFMT64x"\n", addr, addr2);
	r_list_foreach (f1->bbs, iter, bb) {
		if (first) {
			min = bb->addr;
			max = bb->addr + bb->size;
			first = 0;
		} else {
			if (bb->addr < min) {
				min = bb->addr;
			}
			if (bb->addr + bb->size > max) {
				max = bb->addr + bb->size;
			}
		}
	}
	r_list_foreach (f2->bbs, iter, bb) {
		if (first) {
			min = bb->addr;
			max = bb->addr + bb->size;
			first = 0;
		} else {
			if (bb->addr < min) {
				min = bb->addr;
			}
			if (bb->addr + bb->size > max) {
				max = bb->addr + bb->size;
			}
		}
		r_anal_function_add_block (f1, bb);
	}
	// TODO: import data/code/refs
	r_anal_function_delete (f2);
	// update size
	r_anal_function_relocate (f2, R_MIN (addr, addr2));
}