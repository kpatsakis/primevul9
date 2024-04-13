static inline bool get_next_i(IterCtx *ctx, size_t *next_i) {
	(*next_i)++;
	ut64 cur_addr = *next_i + ctx->start_addr;
	if (ctx->fcn) {
		if (!ctx->cur_bb) {
			ctx->path = r_list_new ();
			ctx->switch_path = r_list_new ();
			ctx->bbl = r_list_clone (ctx->fcn->bbs);
			ctx->cur_bb = r_anal_get_block_at (ctx->fcn->anal, ctx->fcn->addr);
			if (!ctx->cur_bb) {
				return false;
			}
			r_list_push (ctx->path, ctx->cur_bb);
		}
		RAnalBlock *bb = ctx->cur_bb;
		if (cur_addr >= bb->addr + bb->size) {
			r_reg_arena_push (ctx->fcn->anal->reg);
			RListIter *bbit = NULL;
			if (bb->switch_op) {
				RAnalCaseOp *cop = r_list_first (bb->switch_op->cases);
				bbit = r_list_find (ctx->bbl, &cop->jump, (RListComparator)find_bb);
				if (bbit) {
					r_list_push (ctx->switch_path, bb->switch_op->cases->head);
				}
			} else {
				bbit = r_list_find (ctx->bbl, &bb->jump, (RListComparator)find_bb);
				if (!bbit && bb->fail != UT64_MAX) {
					bbit = r_list_find (ctx->bbl, &bb->fail, (RListComparator)find_bb);
				}
			}
			if (!bbit) {
				RListIter *cop_it = r_list_last (ctx->switch_path);
				RAnalBlock *prev_bb = NULL;
				do {
					r_reg_arena_pop (ctx->fcn->anal->reg);
					prev_bb = r_list_pop (ctx->path);
					if (prev_bb->fail != UT64_MAX) {
						bbit = r_list_find (ctx->bbl, &prev_bb->fail, (RListComparator)find_bb);
						if (bbit) {
							r_reg_arena_push (ctx->fcn->anal->reg);
							r_list_push (ctx->path, prev_bb);
						}
					}
					if (!bbit && cop_it) {
						RAnalCaseOp *cop = cop_it->data;
						if (cop->jump == prev_bb->addr && cop_it->n) {
							cop = cop_it->n->data;
							r_list_pop (ctx->switch_path);
							r_list_push (ctx->switch_path, cop_it->n);
							cop_it = cop_it->n;
							bbit = r_list_find (ctx->bbl, &cop->jump, (RListComparator)find_bb);
						}
					}
					if (cop_it && !cop_it->n) {
						r_list_pop (ctx->switch_path);
						cop_it = r_list_last (ctx->switch_path);
					}
				} while (!bbit && !r_list_empty (ctx->path));
			}
			if (!bbit) {
				r_list_free (ctx->path);
				r_list_free (ctx->switch_path);
				r_list_free (ctx->bbl);
				ctx->path = NULL;
				ctx->switch_path = NULL;
				ctx->bbl = NULL;
				return false;
			}
			if (!bbit->data) {
				return false;
			}
			if (!bbit->data) {
				return false;
			}
			ctx->cur_bb = bbit->data;
			r_list_push (ctx->path, ctx->cur_bb);
			r_list_delete (ctx->bbl, bbit);
			*next_i = ctx->cur_bb->addr - ctx->start_addr;
		}
	} else if (cur_addr >= ctx->end_addr) {
		return false;
	}
	if (*next_i == 0) {
		return false;
	}
	return true;
}