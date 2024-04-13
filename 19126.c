static void handle_var_stack_access(RAnalEsil *esil, ut64 addr, RAnalVarAccessType type, int len) {
	EsilBreakCtx *ctx = esil->user;
	const char *regname = reg_name_for_access (ctx->op, type);
	if (ctx->fcn && regname) {
		ut64 spaddr = r_reg_getv (esil->anal->reg, ctx->spname);
		if (addr >= spaddr && addr < ctx->initial_sp) {
			int stack_off = addr - ctx->initial_sp;
			RAnalVar *var = r_anal_function_get_var (ctx->fcn, R_ANAL_VAR_KIND_SPV, stack_off);
			if (!var) {
				var = r_anal_function_get_var (ctx->fcn, R_ANAL_VAR_KIND_BPV, stack_off);
			}
			if (!var && stack_off >= -ctx->fcn->maxstack) {
				char *varname;
				varname = ctx->fcn->anal->opt.varname_stack
					? r_str_newf ("var_%xh", R_ABS (stack_off))
					: r_anal_function_autoname_var (ctx->fcn, R_ANAL_VAR_KIND_SPV, "var", delta_for_access (ctx->op, type));
				var = r_anal_function_set_var (ctx->fcn, stack_off, R_ANAL_VAR_KIND_SPV, NULL, len, false, varname);
				free (varname);
			}
			if (var) {
				r_anal_var_set_access (var, regname, ctx->op->addr, type, delta_for_access (ctx->op, type));
			}
		}
	}
}