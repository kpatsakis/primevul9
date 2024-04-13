R_API RAnalOp* r_core_anal_op(RCore *core, ut64 addr, int mask) {
	int len;
	ut8 buf[32];
	ut8 *ptr;

	r_return_val_if_fail (core, NULL);
	if (addr == UT64_MAX) {
		return NULL;
	}
	RAnalOp *op = R_NEW0 (RAnalOp);
	if (!op) {
		return NULL;
	}
	int delta = (addr - core->offset);
	int minopsz = 8;
	if (delta > 0 && delta + minopsz < core->blocksize && addr >= core->offset && addr + 16 < core->offset + core->blocksize) {
		ptr = core->block + delta;
		len = core->blocksize - delta;
		if (len < 1) {
			goto err_op;
		}
	} else {
		if (!r_io_read_at (core->io, addr, buf, sizeof (buf))) {
			goto err_op;
		}
		ptr = buf;
		len = sizeof (buf);
	}
	if (r_anal_op (core->anal, op, addr, ptr, len, mask) < 1) {
		goto err_op;
	}
	// TODO This code block must be deleted when all the anal plugs support disasm
	if (!op->mnemonic && mask & R_ANAL_OP_MASK_DISASM) {
		RAsmOp asmop;
		if (core->anal->verbose) {
			eprintf ("Warning: Implement RAnalOp.MASK_DISASM for current anal.arch. Using the sluggish RAsmOp fallback for now.\n");
		}
		r_asm_set_pc (core->rasm, addr);
		r_asm_op_init (&asmop);
		if (r_asm_disassemble (core->rasm, &asmop, ptr, len) > 0) {
			op->mnemonic = strdup (r_strbuf_get (&asmop.buf_asm));
		}
		r_asm_op_fini (&asmop);
	}
	return op;
err_op:
	free (op);
	return NULL;
}