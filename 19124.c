R_API int r_core_anal_search_xrefs(RCore *core, ut64 from, ut64 to, PJ *pj, int rad) {
	const bool cfg_debug = r_config_get_b (core->config, "cfg.debug");
	bool cfg_anal_strings = r_config_get_i (core->config, "anal.strings");
	ut64 at;
	int count = 0;
	int bsz = 8096;
	RAnalOp op = { 0 };

	if (from == to) {
		return -1;
	}
	if (from > to) {
		eprintf ("Invalid range (0x%"PFMT64x
		" >= 0x%"PFMT64x")\n", from, to);
		return -1;
	}

	if (core->blocksize <= OPSZ) {
		eprintf ("Error: block size too small\n");
		return -1;
	}
	ut8 *buf = malloc (bsz);
	if (!buf) {
		eprintf ("Error: cannot allocate a block\n");
		return -1;
	}
	ut8 *block = malloc (bsz);
	if (!block) {
		eprintf ("Error: cannot allocate a temp block\n");
		free (buf);
		return -1;
	}
	r_cons_break_push (NULL, NULL);
	at = from;
	st64 asm_sub_varmin = r_config_get_i (core->config, "asm.sub.varmin");
	int maxopsz = r_anal_archinfo (core->anal, R_ANAL_ARCHINFO_MAX_OP_SIZE);
	int minopsz = r_anal_archinfo (core->anal, R_ANAL_ARCHINFO_MIN_OP_SIZE);
	if (maxopsz < 1) {
		maxopsz = 4;
	}
	if (minopsz < 1) {
		minopsz = 1;
	}
	if (bsz < maxopsz) {
		// wtf
		eprintf ("Error: Something is really wrong deep inside\n");
		free (block);
		return -1;
	}
	while (at < to && !r_cons_is_breaked ()) {
		int i = 0, ret = bsz;
		if (!r_io_is_valid_offset (core->io, at, R_PERM_X)) {
			break;
		}
		ut64 left = to - at;
		if (bsz > left) {
			bsz = left;
		}
		(void)r_io_read_at (core->io, at, buf, bsz);
		memset (block, -1, bsz);
		if (!memcmp (buf, block, bsz)) {
		//	eprintf ("Error: skipping uninitialized block \n");
			at += ret;
			continue;
		}
		memset (block, 0, bsz);
		if (!memcmp (buf, block, bsz)) {
		//	eprintf ("Error: skipping uninitialized block \n");
			at += ret;
			continue;
		}
		(void) r_anal_op (core->anal, &op, at, buf, bsz, R_ANAL_OP_MASK_BASIC | R_ANAL_OP_MASK_HINT);
		while ((i + maxopsz) < bsz && !r_cons_is_breaked ()) {
			r_anal_op_fini (&op);
			ret = r_anal_op (core->anal, &op, at + i, buf + i, bsz - i, R_ANAL_OP_MASK_BASIC | R_ANAL_OP_MASK_HINT);
			if (ret < 1) {
				i += minopsz;
				continue;
			}
			i += ret;
			if (i > bsz) {
				// at += minopsz;
				break;
			}
			// find references
			if ((st64)op.val > asm_sub_varmin && op.val != UT64_MAX && op.val != UT32_MAX) {
				if (found_xref (core, op.addr, op.val, R_ANAL_REF_TYPE_DATA, pj, rad, cfg_debug, cfg_anal_strings)) {
					count++;
				}
			}
			// find references
			if (op.ptr && op.ptr != UT64_MAX && op.ptr != UT32_MAX) {
				if (found_xref (core, op.addr, op.ptr, R_ANAL_REF_TYPE_DATA, pj, rad, cfg_debug, cfg_anal_strings)) {
					count++;
				}
			}
			// find references
			if (op.addr > 512 && op.disp > 512 && op.disp && op.disp != UT64_MAX) {
				if (found_xref (core, op.addr, op.disp, R_ANAL_REF_TYPE_DATA, pj, rad, cfg_debug, cfg_anal_strings)) {
					count++;
				}
			}
			switch (op.type) {
			case R_ANAL_OP_TYPE_JMP:
			case R_ANAL_OP_TYPE_CJMP:
				if (found_xref (core, op.addr, op.jump, R_ANAL_REF_TYPE_CODE, pj, rad, cfg_debug, cfg_anal_strings)) {
					count++;
				}
				break;
			case R_ANAL_OP_TYPE_CALL:
			case R_ANAL_OP_TYPE_CCALL:
				if (found_xref (core, op.addr, op.jump, R_ANAL_REF_TYPE_CALL, pj, rad, cfg_debug, cfg_anal_strings)) {
					count++;
				}
				break;
			case R_ANAL_OP_TYPE_UJMP:
			case R_ANAL_OP_TYPE_IJMP:
			case R_ANAL_OP_TYPE_RJMP:
			case R_ANAL_OP_TYPE_IRJMP:
			case R_ANAL_OP_TYPE_MJMP:
			case R_ANAL_OP_TYPE_UCJMP:
				count++;
				if (found_xref (core, op.addr, op.ptr, R_ANAL_REF_TYPE_CODE, pj, rad, cfg_debug, cfg_anal_strings)) {
					count++;
				}
				break;
			case R_ANAL_OP_TYPE_UCALL:
			case R_ANAL_OP_TYPE_ICALL:
			case R_ANAL_OP_TYPE_RCALL:
			case R_ANAL_OP_TYPE_IRCALL:
			case R_ANAL_OP_TYPE_UCCALL:
				if (found_xref (core, op.addr, op.ptr, R_ANAL_REF_TYPE_CALL, pj, rad, cfg_debug, cfg_anal_strings)) {
					count++;
				}
				break;
			default:
				break;
			}
		}
		r_anal_op_fini (&op);
		if (i < 1) {
			break;
		}
		at += i + 1;
	}
	r_cons_break_pop ();
	free (buf);
	free (block);
	return count;
}