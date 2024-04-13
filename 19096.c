R_API void r_core_recover_vars(RCore *core, RAnalFunction *fcn, bool argonly) {
	r_return_if_fail (core && core->anal && fcn);
	if (core->anal->opt.bb_max_size < 1) {
		return;
	}
	BlockRecurseCtx ctx = { 0, {{ 0 }}, argonly, fcn, core };
	r_pvector_init (&ctx.reg_set, free);
	int *reg_set = R_NEWS0 (int, REG_SET_SIZE);
	r_pvector_push (&ctx.reg_set, reg_set);
	int saved_stack = fcn->stack;
	RAnalBlock *first_bb = r_anal_get_block_at (fcn->anal, fcn->addr);
	r_anal_block_recurse_depth_first (first_bb, (RAnalBlockCb)anal_block_cb, (RAnalBlockCb)anal_block_on_exit, &ctx);
	r_pvector_fini (&ctx.reg_set);
	fcn->stack = saved_stack;
}