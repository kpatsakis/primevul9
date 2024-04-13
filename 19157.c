static bool analyze_noreturn_function(RCore *core, RAnalFunction *f) {
	RListIter *iter;
	RAnalBlock *bb;
	r_list_foreach (f->bbs, iter, bb) {
		ut64 opaddr = r_anal_bb_opaddr_i (bb, bb->ninstr - 1);
		if (opaddr == UT64_MAX) {
			return false;
		}

		// get last opcode
		RAnalOp *op = r_core_op_anal (core, opaddr, R_ANAL_OP_MASK_HINT);
		if (!op) {
			eprintf ("Cannot analyze opcode at 0x%08" PFMT64x "\n", opaddr);
			return false;
		}

		switch (op->type & R_ANAL_OP_TYPE_MASK) {
		case R_ANAL_OP_TYPE_ILL:
		case R_ANAL_OP_TYPE_RET:
			r_anal_op_free (op);
			return false;
		case R_ANAL_OP_TYPE_JMP:
			if (!r_anal_function_contains (f, op->jump)) {
				r_anal_op_free (op);
				return false;
			}
			break;
		}
		r_anal_op_free (op);
	}
	return true;
}