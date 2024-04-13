static int adjust_scalar_min_max_vals(struct bpf_verifier_env *env,
				      struct bpf_insn *insn,
				      struct bpf_reg_state *dst_reg,
				      struct bpf_reg_state src_reg)
{
	struct bpf_reg_state *regs = cur_regs(env);
	u8 opcode = BPF_OP(insn->code);
	bool src_known;
	s64 smin_val, smax_val;
	u64 umin_val, umax_val;
	s32 s32_min_val, s32_max_val;
	u32 u32_min_val, u32_max_val;
	u64 insn_bitness = (BPF_CLASS(insn->code) == BPF_ALU64) ? 64 : 32;
	u32 dst = insn->dst_reg;
	int ret;
	bool alu32 = (BPF_CLASS(insn->code) != BPF_ALU64);

	smin_val = src_reg.smin_value;
	smax_val = src_reg.smax_value;
	umin_val = src_reg.umin_value;
	umax_val = src_reg.umax_value;

	s32_min_val = src_reg.s32_min_value;
	s32_max_val = src_reg.s32_max_value;
	u32_min_val = src_reg.u32_min_value;
	u32_max_val = src_reg.u32_max_value;

	if (alu32) {
		src_known = tnum_subreg_is_const(src_reg.var_off);
		if ((src_known &&
		     (s32_min_val != s32_max_val || u32_min_val != u32_max_val)) ||
		    s32_min_val > s32_max_val || u32_min_val > u32_max_val) {
			/* Taint dst register if offset had invalid bounds
			 * derived from e.g. dead branches.
			 */
			__mark_reg_unknown(env, dst_reg);
			return 0;
		}
	} else {
		src_known = tnum_is_const(src_reg.var_off);
		if ((src_known &&
		     (smin_val != smax_val || umin_val != umax_val)) ||
		    smin_val > smax_val || umin_val > umax_val) {
			/* Taint dst register if offset had invalid bounds
			 * derived from e.g. dead branches.
			 */
			__mark_reg_unknown(env, dst_reg);
			return 0;
		}
	}

	if (!src_known &&
	    opcode != BPF_ADD && opcode != BPF_SUB && opcode != BPF_AND) {
		__mark_reg_unknown(env, dst_reg);
		return 0;
	}

	/* Calculate sign/unsigned bounds and tnum for alu32 and alu64 bit ops.
	 * There are two classes of instructions: The first class we track both
	 * alu32 and alu64 sign/unsigned bounds independently this provides the
	 * greatest amount of precision when alu operations are mixed with jmp32
	 * operations. These operations are BPF_ADD, BPF_SUB, BPF_MUL, BPF_ADD,
	 * and BPF_OR. This is possible because these ops have fairly easy to
	 * understand and calculate behavior in both 32-bit and 64-bit alu ops.
	 * See alu32 verifier tests for examples. The second class of
	 * operations, BPF_LSH, BPF_RSH, and BPF_ARSH, however are not so easy
	 * with regards to tracking sign/unsigned bounds because the bits may
	 * cross subreg boundaries in the alu64 case. When this happens we mark
	 * the reg unbounded in the subreg bound space and use the resulting
	 * tnum to calculate an approximation of the sign/unsigned bounds.
	 */
	switch (opcode) {
	case BPF_ADD:
		ret = sanitize_val_alu(env, insn);
		if (ret < 0) {
			verbose(env, "R%d tried to add from different pointers or scalars\n", dst);
			return ret;
		}
		scalar32_min_max_add(dst_reg, &src_reg);
		scalar_min_max_add(dst_reg, &src_reg);
		dst_reg->var_off = tnum_add(dst_reg->var_off, src_reg.var_off);
		break;
	case BPF_SUB:
		ret = sanitize_val_alu(env, insn);
		if (ret < 0) {
			verbose(env, "R%d tried to sub from different pointers or scalars\n", dst);
			return ret;
		}
		scalar32_min_max_sub(dst_reg, &src_reg);
		scalar_min_max_sub(dst_reg, &src_reg);
		dst_reg->var_off = tnum_sub(dst_reg->var_off, src_reg.var_off);
		break;
	case BPF_MUL:
		dst_reg->var_off = tnum_mul(dst_reg->var_off, src_reg.var_off);
		scalar32_min_max_mul(dst_reg, &src_reg);
		scalar_min_max_mul(dst_reg, &src_reg);
		break;
	case BPF_AND:
		dst_reg->var_off = tnum_and(dst_reg->var_off, src_reg.var_off);
		scalar32_min_max_and(dst_reg, &src_reg);
		scalar_min_max_and(dst_reg, &src_reg);
		break;
	case BPF_OR:
		dst_reg->var_off = tnum_or(dst_reg->var_off, src_reg.var_off);
		scalar32_min_max_or(dst_reg, &src_reg);
		scalar_min_max_or(dst_reg, &src_reg);
		break;
	case BPF_XOR:
		dst_reg->var_off = tnum_xor(dst_reg->var_off, src_reg.var_off);
		scalar32_min_max_xor(dst_reg, &src_reg);
		scalar_min_max_xor(dst_reg, &src_reg);
		break;
	case BPF_LSH:
		if (umax_val >= insn_bitness) {
			/* Shifts greater than 31 or 63 are undefined.
			 * This includes shifts by a negative number.
			 */
			mark_reg_unknown(env, regs, insn->dst_reg);
			break;
		}
		if (alu32)
			scalar32_min_max_lsh(dst_reg, &src_reg);
		else
			scalar_min_max_lsh(dst_reg, &src_reg);
		break;
	case BPF_RSH:
		if (umax_val >= insn_bitness) {
			/* Shifts greater than 31 or 63 are undefined.
			 * This includes shifts by a negative number.
			 */
			mark_reg_unknown(env, regs, insn->dst_reg);
			break;
		}
		if (alu32)
			scalar32_min_max_rsh(dst_reg, &src_reg);
		else
			scalar_min_max_rsh(dst_reg, &src_reg);
		break;
	case BPF_ARSH:
		if (umax_val >= insn_bitness) {
			/* Shifts greater than 31 or 63 are undefined.
			 * This includes shifts by a negative number.
			 */
			mark_reg_unknown(env, regs, insn->dst_reg);
			break;
		}
		if (alu32)
			scalar32_min_max_arsh(dst_reg, &src_reg);
		else
			scalar_min_max_arsh(dst_reg, &src_reg);
		break;
	default:
		mark_reg_unknown(env, regs, insn->dst_reg);
		break;
	}

	/* ALU32 ops are zero extended into 64bit register */
	if (alu32)
		zext_32_to_64(dst_reg);

	__update_reg_bounds(dst_reg);
	__reg_deduce_bounds(dst_reg);
	__reg_bound_offset(dst_reg);
	return 0;
}