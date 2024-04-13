static bool cmd_aea(RCore* core, int mode, ut64 addr, int length) {
	RAnalEsil *esil;
	int ptr, ops, ops_end = 0, len, buf_sz, maxopsize;
	ut64 addr_end;
	AeaStats stats;
	const char *esilstr;
	RAnalOp aop = R_EMPTY;
	ut8 *buf;
	RList* regnow;
	PJ *pj = NULL;
	if (!core) {
		return false;
	}
	maxopsize = r_anal_archinfo (core->anal, R_ANAL_ARCHINFO_MAX_OP_SIZE);
	if (maxopsize < 1) {
		maxopsize = 16;
	}
	if (mode & 1) {
		// number of bytes / length
		buf_sz = length;
	} else {
		// number of instructions / opcodes
		ops_end = length;
		if (ops_end < 1) {
			ops_end = 1;
		}
		buf_sz = ops_end * maxopsize;
	}
	if (buf_sz < 1) {
		buf_sz = maxopsize;
	}
	addr_end = addr + buf_sz;
	buf = malloc (buf_sz);
	if (!buf) {
		return false;
	}
	(void)r_io_read_at (core->io, addr, (ut8 *)buf, buf_sz);
	aea_stats_init (&stats);

	//esil_init (core);
	//esil = core->anal->esil;
	r_reg_arena_push (core->anal->reg);
	int stacksize = r_config_get_i (core->config, "esil.stack.depth");
	bool iotrap = r_config_get_i (core->config, "esil.iotrap");
	int romem = r_config_get_i (core->config, "esil.romem");
	int stats1 = r_config_get_i (core->config, "esil.stats");
	int noNULL = r_config_get_i (core->config, "esil.noNULL");
	unsigned int addrsize = r_config_get_i (core->config, "esil.addr.size");
	esil = r_anal_esil_new (stacksize, iotrap, addrsize);
	r_anal_esil_setup (esil, core->anal, romem, stats1, noNULL); // setup io
#	define hasNext(x) (x&1) ? (addr<addr_end) : (ops<ops_end)

	mymemxsr = r_list_new ();
	mymemxsw = r_list_new ();
	esil->user = &stats;
	esil->cb.hook_reg_write = myregwrite;
	esil->cb.hook_reg_read = myregread;
	esil->cb.hook_mem_write = mymemwrite;
	esil->cb.hook_mem_read = mymemread;
	esil->nowrite = true;
	for (ops = ptr = 0; ptr < buf_sz && hasNext (mode); ops++, ptr += len) {
		len = r_anal_op (core->anal, &aop, addr + ptr, buf + ptr, buf_sz - ptr, R_ANAL_OP_MASK_ESIL | R_ANAL_OP_MASK_HINT);
		esilstr = R_STRBUF_SAFEGET (&aop.esil);
		if (*esilstr) {
			if (len < 1) {
				eprintf ("Invalid 0x%08"PFMT64x" instruction %02x %02x\n",
					addr + ptr, buf[ptr], buf[ptr + 1]);
				break;
			}
			if (r_config_get_i (core->config, "cfg.r2wars")) {
				if (aop.prefix  & R_ANAL_OP_PREFIX_REP) {
					char * tmp = strstr (esilstr, ",ecx,?{,5,GOTO,}");
					if (tmp) {
						tmp[0] = 0;
					}
				}
			}
			r_anal_esil_parse (esil, esilstr);
			r_anal_esil_stack_free (esil);
		}
		r_anal_op_fini (&aop);
	}
	esil->nowrite = false;
	esil->cb.hook_reg_write = NULL;
	esil->cb.hook_reg_read = NULL;
	//esil_fini (core);
	r_anal_esil_free (esil);
	r_reg_arena_pop (core->anal->reg);
	regnow = r_list_newf (free);
	{
		RListIter *iter;
		char *reg;
		r_list_foreach (stats.regs, iter, reg) {
			if (!contains (stats.regwrite, reg)) {
				r_list_push (regnow, strdup (reg));
			}
		}
	}
	if ((mode >> 5) & 1) {
		RListIter *iter;
		AeaMemItem *n;
		int c = 0;
		r_cons_printf ("f-mem.*\n");
		r_list_foreach (mymemxsr, iter, n) {
			r_cons_printf ("f mem.read.%d 0x%08x @ 0x%08"PFMT64x"\n", c++, n->size, n->addr);
		}
		c = 0;
		r_list_foreach (mymemxsw, iter, n) {
			r_cons_printf ("f mem.write.%d 0x%08x @ 0x%08"PFMT64x"\n", c++, n->size, n->addr);
		}
	}

	/* show registers used */
	if ((mode >> 1) & 1) {
		showregs (stats.regread);
	} else if ((mode >> 2) & 1) {
		showregs (stats.regwrite);
	} else if ((mode >> 3) & 1) {
		showregs (regnow);
	} else if ((mode >> 4) & 1) {
		pj = pj_new ();
		if (!pj) {
			return false;
		}
		pj_o (pj);
		pj_k (pj, "A");
		showregs_json (stats.regs, pj);
		pj_k (pj, "I");
		showregs_json (stats.inputregs, pj);
		pj_k (pj, "R");
		showregs_json (stats.regread, pj);
		pj_k (pj, "W");
		showregs_json (stats.regwrite, pj);
		if (!r_list_empty (stats.regvalues)) {
			pj_k (pj, "V");
			showregs_json (stats.regvalues, pj);
		}
		if (!r_list_empty (regnow)){
			pj_k (pj, "N");
			showregs_json (regnow, pj);
		}
		if (!r_list_empty (mymemxsr)){
			pj_k (pj, "@R");
			showmem_json (mymemxsr, pj);
		}
		if (!r_list_empty (mymemxsw)){
			pj_k (pj, "@W");
			showmem_json (mymemxsw, pj);
		}

		pj_end (pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
	} else if ((mode >> 5) & 1) {
		// nothing
	} else {
		if (!r_list_empty (stats.inputregs)) {
			r_cons_printf (" I: ");
			showregs (stats.inputregs);
		}
		if (!r_list_empty (stats.regs)) {
			r_cons_printf (" A: ");
			showregs (stats.regs);
		}
		if (!r_list_empty (stats.regread)) {
			r_cons_printf (" R: ");
			showregs (stats.regread);
		}
		if (!r_list_empty (stats.regwrite)) {
			r_cons_printf (" W: ");
			showregs (stats.regwrite);
		}
		if (!r_list_empty (stats.regvalues)) {
			r_cons_printf (" V: ");
			showregs (stats.regvalues);
		}
		if (!r_list_empty (regnow)){
			r_cons_printf (" N: ");
			showregs (regnow);
		}
		if (!r_list_empty (mymemxsr)){
			r_cons_printf ("@R:");
			showmem (mymemxsr);
		}
		if (!r_list_empty (mymemxsw)){
			r_cons_printf ("@W:");
			showmem (mymemxsw);
		}
	}

	r_list_free (mymemxsr);
	r_list_free (mymemxsw);
	mymemxsr = NULL;
	mymemxsw = NULL;
	aea_stats_fini (&stats);
	free (buf);
	R_FREE (regnow);
	return true;
}