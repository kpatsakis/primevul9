static bool opiscall(RCore *core, RAnalOp *aop, ut64 addr, const ut8* buf, int len, int arch) {
	switch (arch) {
	case R2_ARCH_ARM64:
		aop->size = 4;
		//addr should be aligned by 4 in aarch64
		if (addr % 4) {
			char diff = addr % 4;
			addr = addr - diff;
			buf = buf - diff;
		}
		//if is not bl do not analyze
		if (buf[3] == 0x94) {
			if (r_anal_op (core->anal, aop, addr, buf, len, R_ANAL_OP_MASK_BASIC)) {
				return true;
			}
		}
		break;
	default:
		aop->size = 1;
		if (r_anal_op (core->anal, aop, addr, buf, len, R_ANAL_OP_MASK_BASIC)) {
			switch (aop->type & R_ANAL_OP_TYPE_MASK) {
			case R_ANAL_OP_TYPE_CALL:
			case R_ANAL_OP_TYPE_CCALL:
				return true;
			}
		}
		break;
	}
	return false;
}