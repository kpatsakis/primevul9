static void _anal_calls(RCore *core, ut64 addr, ut64 addr_end, bool printCommands, bool importsOnly) {
	RAnalOp op;
	int depth = r_config_get_i (core->config, "anal.depth");
	const int addrbytes = core->io->addrbytes;
	const int bsz = 4096;
	int bufi = 0;
	int bufi_max = bsz - 16;
	if (addr_end - addr > UT32_MAX) {
		return;
	}
	ut8 *buf = malloc (bsz);
	ut8 *block0 = calloc (1, bsz);
	ut8 *block1 = malloc (bsz);
	if (!buf || !block0 || !block1) {
		eprintf ("Error: cannot allocate buf or block\n");
		free (buf);
		free (block0);
		free (block1);
		return;
	}
	memset (block1, -1, bsz);
	int minop = r_anal_archinfo (core->anal, R_ANAL_ARCHINFO_MIN_OP_SIZE);
	if (minop < 1) {
		minop = 1;
	}
	int setBits = r_config_get_i (core->config, "asm.bits");
	r_cons_break_push (NULL, NULL);
	while (addr < addr_end && !r_cons_is_breaked ()) {
		// TODO: too many ioreads here
		if (bufi > bufi_max) {
			bufi = 0;
		}
		if (!bufi) {
			(void)r_io_read_at (core->io, addr, buf, bsz);
		}
		if (!memcmp (buf, block0, bsz) || !memcmp (buf, block1, bsz)) {
			//eprintf ("Error: skipping uninitialized block \n");
			addr += bsz;
			continue;
		}
		RAnalHint *hint = r_anal_hint_get (core->anal, addr);
		if (hint && hint->bits) {
			setBits = hint->bits;
		}
		r_anal_hint_free (hint);
		if (setBits != core->assembler->bits) {
			r_config_set_i (core->config, "asm.bits", setBits);
		}
		if (r_anal_op (core->anal, &op, addr, buf + bufi, bsz - bufi, 0) > 0) {
			if (op.size < 1) {
				op.size = minop;
			}
			if (op.type == R_ANAL_OP_TYPE_CALL) {
				bool isValidCall = true;
				if (importsOnly) {
					RFlagItem *f = r_flag_get_i (core->flags, op.jump);
					if (!f || !strstr (f->name, "imp.")) {
						isValidCall = false;
					}
				}
				RBinReloc *rel = r_core_getreloc (core, addr, op.size);
				if (rel && (rel->import || rel->symbol)) {
					isValidCall = false;
				}
				if (isValidCall) {
					ut8 buf[4];
					r_io_read_at (core->io, op.jump, buf, 4);
					isValidCall = memcmp (buf, "\x00\x00\x00\x00", 4);
				}
				if (isValidCall) {
#if JAYRO_03
					if (!anal_is_bad_call (core, from, to, addr, buf, bufi)) {
						fcn = r_anal_get_fcn_in (core->anal, op.jump, R_ANAL_FCN_TYPE_ROOT);
						if (!fcn) {
							r_core_anal_fcn (core, op.jump, addr, R_ANAL_REF_TYPE_CALL, depth);
						}
					}
#else
					if (printCommands) {
						r_cons_printf ("ax 0x%08" PFMT64x " 0x%08" PFMT64x "\n", op.jump, addr);
						r_cons_printf ("af @ 0x%08" PFMT64x"\n", op.jump);
					} else {
						// add xref here
						r_anal_xrefs_set (core->anal, addr, op.jump, R_ANAL_REF_TYPE_CALL);
						if (r_io_is_valid_offset (core->io, op.jump, 1)) {
							r_core_anal_fcn (core, op.jump, addr, R_ANAL_REF_TYPE_CALL, depth);
						}
					}
#endif
				}
			}
		} else {
			op.size = minop;
		}
		if ((int)op.size < 1) {
			op.size = minop;
		}
		addr += op.size;
		bufi += addrbytes * op.size;
		r_anal_op_fini (&op);
	}
	r_cons_break_pop ();
	free (buf);
	free (block0);
	free (block1);
}