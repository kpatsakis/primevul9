static bool esilbreak_reg_write(RAnalEsil *esil, const char *name, ut64 *val) {
	if (!esil) {
		return false;
	}
	RAnal *anal = esil->anal;
	EsilBreakCtx *ctx = esil->user;
	RAnalOp *op = ctx->op;
	RCore *core = anal->coreb.core;
	handle_var_stack_access (esil, *val, R_ANAL_VAR_ACCESS_TYPE_PTR, esil->anal->bits / 8);
	//specific case to handle blx/bx cases in arm through emulation
	// XXX this thing creates a lot of false positives
	ut64 at = *val;
	if (anal && anal->opt.armthumb) {
		if (anal->cur && anal->cur->arch && anal->bits < 33 &&
			strstr (anal->cur->arch, "arm") && !strcmp (name, "pc") && op) {
			switch (op->type) {
			case R_ANAL_OP_TYPE_UCALL: // BLX
			case R_ANAL_OP_TYPE_UJMP: // BX
				// maybe UJMP/UCALL is enough here
				if (!(*val & 1)) {
					r_anal_hint_set_bits (anal, *val, 32);
				} else {
					ut64 snv = r_reg_getv (anal->reg, "pc");
					if (snv != UT32_MAX && snv != UT64_MAX) {
						if (r_io_is_valid_offset (anal->iob.io, *val, 1)) {
							r_anal_hint_set_bits (anal, *val - 1, 16);
						}
					}
				}
				break;
			default:
				break;
			}
		}
	}
	if (core->rasm->bits == 32 && strstr (core->rasm->cur->name, "arm")) {
		if ((!(at & 1)) && r_io_is_valid_offset (anal->iob.io, at, 0)) { //  !core->anal->opt.noncode)) {
			add_string_ref (anal->coreb.core, esil->address, at);
		}
	}
	return 0;
}