static void cmd_aespc(RCore *core, ut64 addr, ut64 until_addr, int off) {
	RAnalEsil *esil = core->anal->esil;
	int i, j = 0;
	ut8 *buf;
	RAnalOp aop = {0};
	int ret , bsize = R_MAX (4096, core->blocksize);
	const int mininstrsz = r_anal_archinfo (core->anal, R_ANAL_ARCHINFO_MIN_OP_SIZE);
	const int minopcode = R_MAX (1, mininstrsz);
	const char *pc = r_reg_get_name (core->dbg->reg, R_REG_NAME_PC);
	int stacksize = r_config_get_i (core->config, "esil.stack.depth");
	int iotrap = r_config_get_i (core->config, "esil.iotrap");
	ut64 addrsize = r_config_get_i (core->config, "esil.addr.size");

	// eprintf ("   aesB %llx %llx %d\n", addr, until_addr, off); // 0x%08llx %d  %s\n", aop.addr, ret, aop.mnemonic);
	if (!esil) {
		eprintf ("Warning: cmd_espc: creating new esil instance\n");
		if (!(esil = r_anal_esil_new (stacksize, iotrap, addrsize))) {
			return;
		}
		core->anal->esil = esil;
	}
	buf = malloc (bsize);
	if (!buf) {
		eprintf ("Cannot allocate %d byte(s)\n", bsize);
		free (buf);
		return;
	}
	if (addr == -1) {
		addr = r_reg_getv (core->dbg->reg, pc);
	}
	(void)r_anal_esil_setup (core->anal->esil, core->anal, 0, 0, 0); // int romem, int stats, int nonull) {
	ut64 cursp = r_reg_getv (core->dbg->reg, "SP");
	ut64 oldoff = core->offset;
	const ut64 flags = R_ANAL_OP_MASK_BASIC | R_ANAL_OP_MASK_HINT | R_ANAL_OP_MASK_ESIL | R_ANAL_OP_MASK_DISASM;
	for (i = 0, j = 0; j < off ; i++, j++) {
		if (r_cons_is_breaked ()) {
			break;
		}
		if (i >= (bsize - 32)) {
			i = 0;
			eprintf ("Warning: Chomp\n");
		}
		if (!i) {
			r_io_read_at (core->io, addr, buf, bsize);
		}
		if (addr == until_addr) {
			break;
		}
		ret = r_anal_op (core->anal, &aop, addr, buf + i, bsize - i, flags);
		if (ret < 1) {
			eprintf ("Failed analysis at 0x%08"PFMT64x"\n", addr);
			break;
		}
		// skip calls and such
		if (aop.type == R_ANAL_OP_TYPE_CALL) {
			// nothing
		} else {
			r_reg_setv (core->anal->reg, "PC", aop.addr + aop.size);
			r_reg_setv (core->dbg->reg, "PC", aop.addr + aop.size);
			const char *e = R_STRBUF_SAFEGET (&aop.esil);
			if (e && *e) {
				 // eprintf ("   0x%08llx %d  %s\n", aop.addr, ret, aop.mnemonic);
				(void)r_anal_esil_parse (esil, e);
			}
		}
		int inc = (core->search->align > 0)? core->search->align - 1: ret - 1;
		if (inc < 0) {
			inc = minopcode;
		}
		i += inc;
		addr += ret; // aop.size;
		r_anal_op_fini (&aop);
	}
	r_core_seek (core, oldoff, 1);
	r_reg_setv (core->dbg->reg, "SP", cursp);
}