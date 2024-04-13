static int decode_modrm(struct x86_emulate_ctxt *ctxt,
			struct x86_emulate_ops *ops)
{
	struct decode_cache *c = &ctxt->decode;
	u8 sib;
	int index_reg = 0, base_reg = 0, scale;
	int rc = 0;

	if (c->rex_prefix) {
		c->modrm_reg = (c->rex_prefix & 4) << 1;	/* REX.R */
		index_reg = (c->rex_prefix & 2) << 2; /* REX.X */
		c->modrm_rm = base_reg = (c->rex_prefix & 1) << 3; /* REG.B */
	}

	c->modrm = insn_fetch(u8, 1, c->eip);
	c->modrm_mod |= (c->modrm & 0xc0) >> 6;
	c->modrm_reg |= (c->modrm & 0x38) >> 3;
	c->modrm_rm |= (c->modrm & 0x07);
	c->modrm_ea = 0;
	c->use_modrm_ea = 1;

	if (c->modrm_mod == 3) {
		c->modrm_ptr = decode_register(c->modrm_rm,
					       c->regs, c->d & ByteOp);
		c->modrm_val = *(unsigned long *)c->modrm_ptr;
		return rc;
	}

	if (c->ad_bytes == 2) {
		unsigned bx = c->regs[VCPU_REGS_RBX];
		unsigned bp = c->regs[VCPU_REGS_RBP];
		unsigned si = c->regs[VCPU_REGS_RSI];
		unsigned di = c->regs[VCPU_REGS_RDI];

		/* 16-bit ModR/M decode. */
		switch (c->modrm_mod) {
		case 0:
			if (c->modrm_rm == 6)
				c->modrm_ea += insn_fetch(u16, 2, c->eip);
			break;
		case 1:
			c->modrm_ea += insn_fetch(s8, 1, c->eip);
			break;
		case 2:
			c->modrm_ea += insn_fetch(u16, 2, c->eip);
			break;
		}
		switch (c->modrm_rm) {
		case 0:
			c->modrm_ea += bx + si;
			break;
		case 1:
			c->modrm_ea += bx + di;
			break;
		case 2:
			c->modrm_ea += bp + si;
			break;
		case 3:
			c->modrm_ea += bp + di;
			break;
		case 4:
			c->modrm_ea += si;
			break;
		case 5:
			c->modrm_ea += di;
			break;
		case 6:
			if (c->modrm_mod != 0)
				c->modrm_ea += bp;
			break;
		case 7:
			c->modrm_ea += bx;
			break;
		}
		if (c->modrm_rm == 2 || c->modrm_rm == 3 ||
		    (c->modrm_rm == 6 && c->modrm_mod != 0))
			if (!c->has_seg_override)
				set_seg_override(c, VCPU_SREG_SS);
		c->modrm_ea = (u16)c->modrm_ea;
	} else {
		/* 32/64-bit ModR/M decode. */
		if ((c->modrm_rm & 7) == 4) {
			sib = insn_fetch(u8, 1, c->eip);
			index_reg |= (sib >> 3) & 7;
			base_reg |= sib & 7;
			scale = sib >> 6;

			if ((base_reg & 7) == 5 && c->modrm_mod == 0)
				c->modrm_ea += insn_fetch(s32, 4, c->eip);
			else
				c->modrm_ea += c->regs[base_reg];
			if (index_reg != 4)
				c->modrm_ea += c->regs[index_reg] << scale;
		} else if ((c->modrm_rm & 7) == 5 && c->modrm_mod == 0) {
			if (ctxt->mode == X86EMUL_MODE_PROT64)
				c->rip_relative = 1;
		} else
			c->modrm_ea += c->regs[c->modrm_rm];
		switch (c->modrm_mod) {
		case 0:
			if (c->modrm_rm == 5)
				c->modrm_ea += insn_fetch(s32, 4, c->eip);
			break;
		case 1:
			c->modrm_ea += insn_fetch(s8, 1, c->eip);
			break;
		case 2:
			c->modrm_ea += insn_fetch(s32, 4, c->eip);
			break;
		}
	}
done:
	return rc;
}