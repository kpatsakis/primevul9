static void analPaths(RCoreAnalPaths *p, PJ *pj) {
	RAnalBlock *cur = p->cur;
	if (!cur) {
		// eprintf ("eof\n");
		return;
	}
	/* handle ^C */
	if (r_cons_is_breaked ()) {
		return;
	}
	dict_set (&p->visited, cur->addr, 1, NULL);
	r_list_append (p->path, cur);
	if (p->followDepth && --p->followDepth == 0) {
		return;
	}
	if (p->toBB && cur->addr == p->toBB->addr) {
		if (!printAnalPaths (p, pj)) {
			return;
		}
	} else {
		RAnalBlock *c = cur;
		ut64 j = cur->jump;
		ut64 f = cur->fail;
		analPathFollow (p, j, pj);
		cur = c;
		analPathFollow (p, f, pj);
		if (p->followCalls) {
			int i;
			for (i = 0; i < cur->op_pos_size; i++) {
				ut64 addr = cur->addr + cur->op_pos[i];
				RAnalOp *op = r_core_anal_op (p->core, addr, R_ANAL_OP_MASK_BASIC);
				if (op && op->type == R_ANAL_OP_TYPE_CALL) {
					analPathFollow (p, op->jump, pj);
				}
				cur = c;
				r_anal_op_free (op);
			}
		}
	}
	p->cur = r_list_pop (p->path);
	dict_del (&p->visited, cur->addr);
	if (p->followDepth) {
		p->followDepth++;
	}
}