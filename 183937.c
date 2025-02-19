x86_decode_insn(struct x86_emulate_ctxt *ctxt, struct x86_emulate_ops *ops)
{
	struct decode_cache *c = &ctxt->decode;
	int rc = 0;
	int mode = ctxt->mode;
	int def_op_bytes, def_ad_bytes, group;

	/* Shadow copy of register state. Committed on successful emulation. */

	memset(c, 0, sizeof(struct decode_cache));
	c->eip = c->eip_orig = kvm_rip_read(ctxt->vcpu);
	ctxt->cs_base = seg_base(ctxt, VCPU_SREG_CS);
	memcpy(c->regs, ctxt->vcpu->arch.regs, sizeof c->regs);

	switch (mode) {
	case X86EMUL_MODE_REAL:
	case X86EMUL_MODE_PROT16:
		def_op_bytes = def_ad_bytes = 2;
		break;
	case X86EMUL_MODE_PROT32:
		def_op_bytes = def_ad_bytes = 4;
		break;
#ifdef CONFIG_X86_64
	case X86EMUL_MODE_PROT64:
		def_op_bytes = 4;
		def_ad_bytes = 8;
		break;
#endif
	default:
		return -1;
	}

	c->op_bytes = def_op_bytes;
	c->ad_bytes = def_ad_bytes;

	/* Legacy prefixes. */
	for (;;) {
		switch (c->b = insn_fetch(u8, 1, c->eip)) {
		case 0x66:	/* operand-size override */
			/* switch between 2/4 bytes */
			c->op_bytes = def_op_bytes ^ 6;
			break;
		case 0x67:	/* address-size override */
			if (mode == X86EMUL_MODE_PROT64)
				/* switch between 4/8 bytes */
				c->ad_bytes = def_ad_bytes ^ 12;
			else
				/* switch between 2/4 bytes */
				c->ad_bytes = def_ad_bytes ^ 6;
			break;
		case 0x26:	/* ES override */
		case 0x2e:	/* CS override */
		case 0x36:	/* SS override */
		case 0x3e:	/* DS override */
			set_seg_override(c, (c->b >> 3) & 3);
			break;
		case 0x64:	/* FS override */
		case 0x65:	/* GS override */
			set_seg_override(c, c->b & 7);
			break;
		case 0x40 ... 0x4f: /* REX */
			if (mode != X86EMUL_MODE_PROT64)
				goto done_prefixes;
			c->rex_prefix = c->b;
			continue;
		case 0xf0:	/* LOCK */
			c->lock_prefix = 1;
			break;
		case 0xf2:	/* REPNE/REPNZ */
			c->rep_prefix = REPNE_PREFIX;
			break;
		case 0xf3:	/* REP/REPE/REPZ */
			c->rep_prefix = REPE_PREFIX;
			break;
		default:
			goto done_prefixes;
		}

		/* Any legacy prefix after a REX prefix nullifies its effect. */

		c->rex_prefix = 0;
	}

done_prefixes:

	/* REX prefix. */
	if (c->rex_prefix)
		if (c->rex_prefix & 8)
			c->op_bytes = 8;	/* REX.W */

	/* Opcode byte(s). */
	c->d = opcode_table[c->b];
	if (c->d == 0) {
		/* Two-byte opcode? */
		if (c->b == 0x0f) {
			c->twobyte = 1;
			c->b = insn_fetch(u8, 1, c->eip);
			c->d = twobyte_table[c->b];
		}
	}

	if (mode == X86EMUL_MODE_PROT64 && (c->d & No64)) {
		kvm_report_emulation_failure(ctxt->vcpu, "invalid x86/64 instruction");;
		return -1;
	}

	if (c->d & Group) {
		group = c->d & GroupMask;
		c->modrm = insn_fetch(u8, 1, c->eip);
		--c->eip;

		group = (group << 3) + ((c->modrm >> 3) & 7);
		if ((c->d & GroupDual) && (c->modrm >> 6) == 3)
			c->d = group2_table[group];
		else
			c->d = group_table[group];
	}

	/* Unrecognised? */
	if (c->d == 0) {
		DPRINTF("Cannot emulate %02x\n", c->b);
		return -1;
	}

	if (mode == X86EMUL_MODE_PROT64 && (c->d & Stack))
		c->op_bytes = 8;

	/* ModRM and SIB bytes. */
	if (c->d & ModRM)
		rc = decode_modrm(ctxt, ops);
	else if (c->d & MemAbs)
		rc = decode_abs(ctxt, ops);
	if (rc)
		goto done;

	if (!c->has_seg_override)
		set_seg_override(c, VCPU_SREG_DS);

	if (!(!c->twobyte && c->b == 0x8d))
		c->modrm_ea += seg_override_base(ctxt, c);

	if (c->ad_bytes != 8)
		c->modrm_ea = (u32)c->modrm_ea;
	/*
	 * Decode and fetch the source operand: register, memory
	 * or immediate.
	 */
	switch (c->d & SrcMask) {
	case SrcNone:
		break;
	case SrcReg:
		decode_register_operand(&c->src, c, 0);
		break;
	case SrcMem16:
		c->src.bytes = 2;
		goto srcmem_common;
	case SrcMem32:
		c->src.bytes = 4;
		goto srcmem_common;
	case SrcMem:
		c->src.bytes = (c->d & ByteOp) ? 1 :
							   c->op_bytes;
		/* Don't fetch the address for invlpg: it could be unmapped. */
		if (c->twobyte && c->b == 0x01 && c->modrm_reg == 7)
			break;
	srcmem_common:
		/*
		 * For instructions with a ModR/M byte, switch to register
		 * access if Mod = 3.
		 */
		if ((c->d & ModRM) && c->modrm_mod == 3) {
			c->src.type = OP_REG;
			c->src.val = c->modrm_val;
			c->src.ptr = c->modrm_ptr;
			break;
		}
		c->src.type = OP_MEM;
		break;
	case SrcImm:
	case SrcImmU:
		c->src.type = OP_IMM;
		c->src.ptr = (unsigned long *)c->eip;
		c->src.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
		if (c->src.bytes == 8)
			c->src.bytes = 4;
		/* NB. Immediates are sign-extended as necessary. */
		switch (c->src.bytes) {
		case 1:
			c->src.val = insn_fetch(s8, 1, c->eip);
			break;
		case 2:
			c->src.val = insn_fetch(s16, 2, c->eip);
			break;
		case 4:
			c->src.val = insn_fetch(s32, 4, c->eip);
			break;
		}
		if ((c->d & SrcMask) == SrcImmU) {
			switch (c->src.bytes) {
			case 1:
				c->src.val &= 0xff;
				break;
			case 2:
				c->src.val &= 0xffff;
				break;
			case 4:
				c->src.val &= 0xffffffff;
				break;
			}
		}
		break;
	case SrcImmByte:
	case SrcImmUByte:
		c->src.type = OP_IMM;
		c->src.ptr = (unsigned long *)c->eip;
		c->src.bytes = 1;
		if ((c->d & SrcMask) == SrcImmByte)
			c->src.val = insn_fetch(s8, 1, c->eip);
		else
			c->src.val = insn_fetch(u8, 1, c->eip);
		break;
	case SrcOne:
		c->src.bytes = 1;
		c->src.val = 1;
		break;
	}

	/*
	 * Decode and fetch the second source operand: register, memory
	 * or immediate.
	 */
	switch (c->d & Src2Mask) {
	case Src2None:
		break;
	case Src2CL:
		c->src2.bytes = 1;
		c->src2.val = c->regs[VCPU_REGS_RCX] & 0x8;
		break;
	case Src2ImmByte:
		c->src2.type = OP_IMM;
		c->src2.ptr = (unsigned long *)c->eip;
		c->src2.bytes = 1;
		c->src2.val = insn_fetch(u8, 1, c->eip);
		break;
	case Src2Imm16:
		c->src2.type = OP_IMM;
		c->src2.ptr = (unsigned long *)c->eip;
		c->src2.bytes = 2;
		c->src2.val = insn_fetch(u16, 2, c->eip);
		break;
	case Src2One:
		c->src2.bytes = 1;
		c->src2.val = 1;
		break;
	}

	/* Decode and fetch the destination operand: register or memory. */
	switch (c->d & DstMask) {
	case ImplicitOps:
		/* Special instructions do their own operand decoding. */
		return 0;
	case DstReg:
		decode_register_operand(&c->dst, c,
			 c->twobyte && (c->b == 0xb6 || c->b == 0xb7));
		break;
	case DstMem:
		if ((c->d & ModRM) && c->modrm_mod == 3) {
			c->dst.bytes = (c->d & ByteOp) ? 1 : c->op_bytes;
			c->dst.type = OP_REG;
			c->dst.val = c->dst.orig_val = c->modrm_val;
			c->dst.ptr = c->modrm_ptr;
			break;
		}
		c->dst.type = OP_MEM;
		break;
	case DstAcc:
		c->dst.type = OP_REG;
		c->dst.bytes = c->op_bytes;
		c->dst.ptr = &c->regs[VCPU_REGS_RAX];
		switch (c->op_bytes) {
			case 1:
				c->dst.val = *(u8 *)c->dst.ptr;
				break;
			case 2:
				c->dst.val = *(u16 *)c->dst.ptr;
				break;
			case 4:
				c->dst.val = *(u32 *)c->dst.ptr;
				break;
		}
		c->dst.orig_val = c->dst.val;
		break;
	}

	if (c->rip_relative)
		c->modrm_ea += c->eip;

done:
	return (rc == X86EMUL_UNHANDLEABLE) ? -1 : 0;
}