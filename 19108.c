R_API int r_core_anal_search(RCore *core, ut64 from, ut64 to, ut64 ref, int mode) {
	ut8 *buf = (ut8 *)malloc (core->blocksize);
	if (!buf) {
		return -1;
	}
	int ptrdepth = r_config_get_i (core->config, "anal.ptrdepth");
	int i, count = 0;
	RAnalOp op = R_EMPTY;
	ut64 at;
	char bckwrds, do_bckwrd_srch;
	int arch = -1;
	if (core->rasm->bits == 64) {
		// speedup search
		if (!strncmp (core->rasm->cur->name, "arm", 3)) {
			arch = R2_ARCH_ARM64;
		}
	}
	// TODO: get current section range here or gtfo
	// ???
	// XXX must read bytes correctly
	do_bckwrd_srch = bckwrds = core->search->bckwrds;
	if (!ref) {
		eprintf ("Null reference search is not supported\n");
		free (buf);
		return -1;
	}
	r_cons_break_push (NULL, NULL);
	if (core->blocksize > OPSZ) {
		if (bckwrds) {
			if (from + core->blocksize > to) {
				at = from;
				do_bckwrd_srch = false;
			} else {
				at = to - core->blocksize;
			}
		} else {
			at = from;
		}
		while ((!bckwrds && at < to) || bckwrds) {
			eprintf ("\r[0x%08"PFMT64x"-0x%08"PFMT64x"] ", at, to);
			if (r_cons_is_breaked ()) {
				break;
			}
			// TODO: this can be probably enhanced
			if (!r_io_read_at (core->io, at, buf, core->blocksize)) {
				eprintf ("Failed to read at 0x%08" PFMT64x "\n", at);
				break;
			}
			for (i = bckwrds ? (core->blocksize - OPSZ - 1) : 0;
				 (!bckwrds && i < core->blocksize - OPSZ) ||
				 (bckwrds && i > 0);
				 bckwrds ? i-- : i++) {
				// TODO: honor anal.align
				if (r_cons_is_breaked ()) {
					break;
				}
				switch (mode) {
				case 'c':
					(void)opiscall (core, &op, at + i, buf + i, core->blocksize - i, arch);
					if (op.size < 1) {
						op.size = 1;
					}
					break;
				case 'r':
				case 'w':
				case 'x':
					{
						r_anal_op (core->anal, &op, at + i, buf + i, core->blocksize - i, R_ANAL_OP_MASK_BASIC);
						int mask = mode=='r' ? 1 : mode == 'w' ? 2: mode == 'x' ? 4: 0;
						if (op.direction == mask) {
							i += op.size;
						}
						r_anal_op_fini (&op);
						continue;
					}
					break;
				default:
					if (!r_anal_op (core->anal, &op, at + i, buf + i, core->blocksize - i, R_ANAL_OP_MASK_BASIC)) {
						r_anal_op_fini (&op);
						continue;
					}
				}
				switch (op.type) {
				case R_ANAL_OP_TYPE_JMP:
				case R_ANAL_OP_TYPE_CJMP:
				case R_ANAL_OP_TYPE_CALL:
				case R_ANAL_OP_TYPE_CCALL:
					if (op.jump != UT64_MAX &&
						core_anal_followptr (core, 'C', at + i, op.jump, ref, true, 0)) {
						count ++;
					}
					break;
				case R_ANAL_OP_TYPE_UCJMP:
				case R_ANAL_OP_TYPE_UJMP:
				case R_ANAL_OP_TYPE_IJMP:
				case R_ANAL_OP_TYPE_RJMP:
				case R_ANAL_OP_TYPE_IRJMP:
				case R_ANAL_OP_TYPE_MJMP:
					if (op.ptr != UT64_MAX &&
						core_anal_followptr (core, 'c', at + i, op.ptr, ref, true ,1)) {
						count ++;
					}
					break;
				case R_ANAL_OP_TYPE_UCALL:
				case R_ANAL_OP_TYPE_ICALL:
				case R_ANAL_OP_TYPE_RCALL:
				case R_ANAL_OP_TYPE_IRCALL:
				case R_ANAL_OP_TYPE_UCCALL:
					if (op.ptr != UT64_MAX &&
						core_anal_followptr (core, 'C', at + i, op.ptr, ref, true ,1)) {
						count ++;
					}
					break;
				default:
					{
						if (!r_anal_op (core->anal, &op, at + i, buf + i, core->blocksize - i, R_ANAL_OP_MASK_BASIC)) {
							r_anal_op_fini (&op);
							continue;
						}
					}
					if (op.ptr != UT64_MAX &&
						core_anal_followptr (core, 'd', at + i, op.ptr, ref, false, ptrdepth)) {
						count ++;
					}
					break;
				}
				if (op.size < 1) {
					op.size = 1;
				}
				i += op.size - 1;
				r_anal_op_fini (&op);
			}
			if (bckwrds) {
				if (!do_bckwrd_srch) {
					break;
				}
				if (at > from + core->blocksize - OPSZ) {
					at -= core->blocksize;
				} else {
					do_bckwrd_srch = false;
					at = from;
				}
			} else {
				at += core->blocksize - OPSZ;
			}
		}
	} else {
		eprintf ("error: block size too small\n");
	}
	r_cons_break_pop ();
	free (buf);
	r_anal_op_fini (&op);
	return count;
}