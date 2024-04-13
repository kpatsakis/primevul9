static bool anal_block_cb(RAnalBlock *bb, BlockRecurseCtx *ctx) {
	if (r_cons_is_breaked ()) {
		return false;
	}
	if (bb->size < 1) {
		return true;
	}
	if (bb->size > ctx->core->anal->opt.bb_max_size) {
		return true;
	}
	int *parent_reg_set = r_pvector_at (&ctx->reg_set, r_pvector_len (&ctx->reg_set) - 1);
	int *reg_set = R_NEWS (int, REG_SET_SIZE);
	memcpy (reg_set, parent_reg_set, REG_SET_SIZE * sizeof (int));
	r_pvector_push (&ctx->reg_set, reg_set);
	RCore *core = ctx->core;
	RAnalFunction *fcn = ctx->fcn;
	fcn->stack = bb->parent_stackptr;
	ut64 pos = bb->addr;
	while (pos < bb->addr + bb->size) {
		if (r_cons_is_breaked ()) {
			break;
		}
		RAnalOp *op = r_core_anal_op (core, pos, R_ANAL_OP_MASK_ESIL | R_ANAL_OP_MASK_VAL | R_ANAL_OP_MASK_HINT);
		if (!op) {
			//eprintf ("Cannot get op\n");
			break;
		}
		r_anal_extract_rarg (core->anal, op, fcn, reg_set, &ctx->count);
		if (!ctx->argonly) {
			if (op->stackop == R_ANAL_STACK_INC) {
				fcn->stack += op->stackptr;
			} else if (op->stackop == R_ANAL_STACK_RESET) {
				fcn->stack = 0;
			}
			r_anal_extract_vars (core->anal, fcn, op);
		}
		int opsize = op->size;
		int optype = op->type;
		r_anal_op_free (op);
		if (opsize < 1) {
			break;
		}
		if (optype == R_ANAL_OP_TYPE_CALL) {
			size_t i;
			int max_count = fcn->cc ? r_anal_cc_max_arg (core->anal, fcn->cc) : 0;
			for (i = 0; i < max_count; i++) {
				reg_set[i] = 2;
			}
		}
		pos += opsize;
	}
	return true;
}