R_API int r_core_esil_step(RCore *core, ut64 until_addr, const char *until_expr, ut64 *prev_addr, bool stepOver) {
#define return_tail(x) { tail_return_value = x; goto tail_return; }
	int tail_return_value = 0;
	int ret;
	ut8 code[32];
	RAnalOp op = {0};
	RAnalEsil *esil = core->anal->esil;
	const char *name = r_reg_get_name (core->anal->reg, R_REG_NAME_PC);
	ut64 addr;
	bool breakoninvalid = r_config_get_i (core->config, "esil.breakoninvalid");
	int esiltimeout = r_config_get_i (core->config, "esil.timeout");
	ut64 startTime;

	if (esiltimeout > 0) {
		startTime = r_sys_now ();
	}
	r_cons_break_push (NULL, NULL);
repeat:
	if (r_cons_is_breaked ()) {
		eprintf ("[+] ESIL emulation interrupted at 0x%08" PFMT64x "\n", addr);
		return_tail (0);
	}
	//Break if we have exceeded esil.timeout
	if (esiltimeout > 0) {
		ut64 elapsedTime = r_sys_now () - startTime;
		elapsedTime >>= 20;
		if (elapsedTime >= esiltimeout) {
			eprintf ("[ESIL] Timeout exceeded.\n");
			return_tail (0);
		}
	}
	if (!esil) {
		addr = initializeEsil (core);
		esil = core->anal->esil;
		if (!esil) {
			return_tail (0);
		}
	} else {
		esil->trap = 0;
		addr = r_reg_getv (core->anal->reg, name);
		//eprintf ("PC=0x%"PFMT64x"\n", (ut64)addr);
	}
	if (prev_addr) {
		*prev_addr = addr;
	}
	if (esil->exectrap) {
		if (!r_io_is_valid_offset (core->io, addr, R_PERM_X)) {
			esil->trap = R_ANAL_TRAP_EXEC_ERR;
			esil->trap_code = addr;
			eprintf ("[ESIL] Trap, trying to execute on non-executable memory\n");
			return_tail (1);
		}
	}
	r_asm_set_pc (core->assembler, addr);
	// run esil pin command here
	const char *pincmd = r_anal_pin_call (core->anal, addr);
	if (pincmd) {
		r_core_cmd0 (core, pincmd);
		ut64 pc = r_debug_reg_get (core->dbg, "PC");
		if (addr != pc) {
			return_tail (1);
		}
	}
	int dataAlign = r_anal_archinfo (esil->anal, R_ANAL_ARCHINFO_DATA_ALIGN);
	if (dataAlign > 1) {
		if (addr % dataAlign) {
			if (esil->cmd && esil->cmd_trap) {
				esil->cmd (esil, esil->cmd_trap, addr, R_ANAL_TRAP_UNALIGNED);
			}
			if (breakoninvalid) {
				r_cons_printf ("[ESIL] Stopped execution in an unaligned instruction (see e??esil.breakoninvalid)\n");
				return_tail (0);
			}
		}
	}
	(void) r_io_read_at_mapped (core->io, addr, code, sizeof (code));
	// TODO: sometimes this is dupe
	ret = r_anal_op (core->anal, &op, addr, code, sizeof (code), R_ANAL_OP_MASK_ESIL | R_ANAL_OP_MASK_HINT);
	// if type is JMP then we execute the next N instructions
	// update the esil pointer because RAnal.op() can change it
	esil = core->anal->esil;
	if (op.size < 1 || ret < 1) {
		if (esil->cmd && esil->cmd_trap) {
			esil->cmd (esil, esil->cmd_trap, addr, R_ANAL_TRAP_INVALID);
		}
		if (breakoninvalid) {
			eprintf ("[ESIL] Stopped execution in an invalid instruction (see e??esil.breakoninvalid)\n");
			return_tail (0);
		}
		op.size = 1; // avoid inverted stepping
	}
	if (stepOver) {
		switch (op.type) {
		case R_ANAL_OP_TYPE_SWI:
		case R_ANAL_OP_TYPE_UCALL:
		case R_ANAL_OP_TYPE_CALL:
		case R_ANAL_OP_TYPE_JMP:
		case R_ANAL_OP_TYPE_RCALL:
		case R_ANAL_OP_TYPE_RJMP:
		case R_ANAL_OP_TYPE_CJMP:
		case R_ANAL_OP_TYPE_RET:
		case R_ANAL_OP_TYPE_CRET:
		case R_ANAL_OP_TYPE_UJMP:
			if (addr == until_addr) {
				return_tail (0);
			} else {
				r_reg_setv (core->anal->reg, "PC", op.addr + op.size);
				r_reg_setv (core->dbg->reg, "PC", op.addr + op.size);
			}
			return 1;
		}
	}
	if (r_config_get_i (core->config, "cfg.r2wars")) {
		// this is x86 and r2wars specific, shouldnt hurt outside x86
		ut64 vECX = r_reg_getv (core->anal->reg, "ecx");
		if (op.prefix  & R_ANAL_OP_PREFIX_REP && vECX > 1) {
			char *tmp = strstr (op.esil.ptr, ",ecx,?{,5,GOTO,}");
			if (tmp) {
				tmp[0] = 0;
			}
			op.esil.len -= 16;
		} else {
			r_reg_setv (core->anal->reg, name, addr + op.size);
		}
	} else {
		r_reg_setv (core->anal->reg, name, addr + op.size);
	}
	if (ret) {
		r_anal_esil_set_pc (esil, addr);
		if (core->dbg->trace->enabled) {
			RReg *reg = core->dbg->reg;
			core->dbg->reg = core->anal->reg;
			r_debug_trace_pc (core->dbg, addr);
			core->dbg->reg = reg;
		} else if (R_STR_ISNOTEMPTY (R_STRBUF_SAFEGET (&op.esil))) {
			r_anal_esil_parse (esil, R_STRBUF_SAFEGET (&op.esil));
			if (core->anal->cur && core->anal->cur->esil_post_loop) {
				core->anal->cur->esil_post_loop (esil, &op);
			}
			r_anal_esil_stack_free (esil);
		}
		bool isNextFall = false;
		if (op.type == R_ANAL_OP_TYPE_CJMP) {
			ut64 pc = r_debug_reg_get (core->dbg, "PC");
			if (pc == addr + op.size) {
				// do not opdelay here
				isNextFall = true;
			}
		}
		// only support 1 slot for now
		if (op.delay && !isNextFall) {
			ut8 code2[32];
			ut64 naddr = addr + op.size;
			RAnalOp op2 = {0};
			// emulate only 1 instruction
			r_anal_esil_set_pc (esil, naddr);
			(void)r_io_read_at (core->io, naddr, code2, sizeof (code2));
			// TODO: sometimes this is dupe
			ret = r_anal_op (core->anal, &op2, naddr, code2, sizeof (code2), R_ANAL_OP_MASK_ESIL | R_ANAL_OP_MASK_HINT);
			if (ret > 0) {
				switch (op2.type) {
				case R_ANAL_OP_TYPE_CJMP:
				case R_ANAL_OP_TYPE_JMP:
				case R_ANAL_OP_TYPE_CRET:
				case R_ANAL_OP_TYPE_RET:
					// branches are illegal in a delay slot
					esil->trap = R_ANAL_TRAP_EXEC_ERR;
					esil->trap_code = addr;
					eprintf ("[ESIL] Trap, trying to execute a branch in a delay slot\n");
					return_tail (1);
					break;
				}
				r_anal_esil_parse (esil, R_STRBUF_SAFEGET (&op2.esil));
			} else {
				eprintf ("Invalid instruction at 0x%08"PFMT64x"\n", naddr);
			}
			r_anal_op_fini (&op2);
		}
		tail_return_value = 1;
	}
	// esil->verbose ?
	// eprintf ("REPE 0x%llx %s => 0x%llx\n", addr, R_STRBUF_SAFEGET (&op.esil), r_reg_getv (core->anal->reg, "PC"));

	st64 follow = (st64)r_config_get_i (core->config, "dbg.follow");
	if (follow > 0) {
		ut64 pc = r_debug_reg_get (core->dbg, "PC");
		if ((pc < core->offset) || (pc > (core->offset + follow))) {
			r_core_cmd0 (core, "sr PC");
		}
	}
	// check breakpoints
	ut64 pc = r_reg_getv (core->anal->reg, name);
	if (r_bp_get_at (core->dbg->bp, pc)) {
		r_cons_printf ("[ESIL] hit breakpoint at 0x%"PFMT64x "\n", pc);
		return_tail (0);
	}
	// check addr
	if (until_addr != UT64_MAX) {
		if (pc == until_addr) {
			return_tail (0);
		}
		goto repeat;
	}
	// check esil
	if (esil && esil->trap) {
		if (core->anal->esil->verbose) {
			eprintf ("TRAP\n");
		}
		return_tail (0);
	}
	if (until_expr) {
		if (r_anal_esil_condition (core->anal->esil, until_expr)) {
			if (core->anal->esil->verbose) {
				eprintf ("ESIL BREAK!\n");
			}
			return_tail (0);
		}
		goto repeat;
	}
tail_return:
	r_anal_op_fini (&op);
	r_cons_break_pop ();
	return tail_return_value;
}