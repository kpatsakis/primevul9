static bool anal_block_on_exit(RAnalBlock *bb, BlockRecurseCtx *ctx) {
	int *cur_regset = r_pvector_pop (&ctx->reg_set);
	int *prev_regset = r_pvector_at (&ctx->reg_set, r_pvector_len (&ctx->reg_set) - 1);
	size_t i;
	for (i = 0; i < REG_SET_SIZE; i++) {
		if (!prev_regset[i] && cur_regset[i] == 1) {
			prev_regset[i] = 1;
		}
	}
	free (cur_regset);
	return true;
}