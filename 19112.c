static bool anal_path_exists(RCore *core, ut64 from, ut64 to, RList *bbs, int depth, HtUP *state, HtUP *avoid) {
	r_return_val_if_fail (bbs, false);
	RAnalBlock *bb = r_anal_bb_from_offset (core->anal, from);
	RListIter *iter = NULL;
	RAnalRef *refi;

	if (depth < 0) {
		eprintf ("going too deep\n");
		return false;
	}

	if (!bb) {
		return false;
	}

	ht_up_update (state, from, bb);

	// try to find the target in the current function
	if (r_anal_block_contains (bb, to) ||
		((!ht_up_find (avoid, bb->jump, NULL) &&
			!ht_up_find (state, bb->jump, NULL) &&
			anal_path_exists (core, bb->jump, to, bbs, depth - 1, state, avoid))) ||
		((!ht_up_find (avoid, bb->fail, NULL) &&
			!ht_up_find (state, bb->fail, NULL) &&
			anal_path_exists (core, bb->fail, to, bbs, depth - 1, state, avoid)))) {
		r_list_prepend (bbs, bb);
		return true;
	}

	// find our current function
	RAnalFunction *cur_fcn = r_anal_get_fcn_in (core->anal, from, 0);

	// get call refs from current basic block and find a path from them
	if (cur_fcn) {
		RList *refs = r_anal_function_get_refs (cur_fcn);
		if (refs) {
			r_list_foreach (refs, iter, refi) {
				if (refi->type == R_ANAL_REF_TYPE_CALL) {
					if (r_anal_block_contains (bb, refi->at)) {
						if ((refi->at != refi->addr) && !ht_up_find (state, refi->addr, NULL) && anal_path_exists (core, refi->addr, to, bbs, depth - 1, state, avoid)) {
							r_list_prepend (bbs, bb);
							r_list_free (refs);
							return true;
						}
					}
				}
			}
		}
		r_list_free (refs);
	}

	return false;
}