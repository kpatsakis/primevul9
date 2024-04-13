static void cmd_esil_mem(RCore *core, const char *input) {
	RAnalEsil *esil = core->anal->esil;
	RIOMap *stack_map;
	ut64 curoff = core->offset;
	const char *patt = "";
	ut64 addr = 0x100000;
	ut32 size = 0xf0000;
	char name[128];
	RFlagItem *fi;
	const char *sp, *bp, *pc;
	char uri[32];
	char nomalloc[256];
	char *p;
	if (!esil) {
		int stacksize = r_config_get_i (core->config, "esil.stack.depth");
		int iotrap = r_config_get_i (core->config, "esil.iotrap");
		int romem = r_config_get_i (core->config, "esil.romem");
		int stats = r_config_get_i (core->config, "esil.stats");
		int noNULL = r_config_get_i (core->config, "esil.noNULL");
		int verbose = r_config_get_i (core->config, "esil.verbose");
		unsigned int addrsize = r_config_get_i (core->config, "esil.addr.size");
		if (!(esil = r_anal_esil_new (stacksize, iotrap, addrsize))) {
			return;
		}
		r_anal_esil_setup (esil, core->anal, romem, stats, noNULL); // setup io
		core->anal->esil = esil;
		esil->verbose = verbose;
		{
			const char *s = r_config_get (core->config, "cmd.esil.intr");
			if (s) {
				char *my = strdup (s);
				if (my) {
					r_config_set (core->config, "cmd.esil.intr", my);
					free (my);
				}
			}
		}
	}
	if (*input == '?') {
		eprintf ("Usage: aeim [addr] [size] [name] - initialize ESIL VM stack\n");
		eprintf ("Default: 0x100000 0xf0000\n");
		eprintf ("See ae? for more help\n");
		return;
	}

	if (input[0] == 'p') {
		fi = r_flag_get (core->flags, "aeim.stack");
		if (fi) {
			addr = fi->offset;
			size = fi->size;
		} else {
			cmd_esil_mem (core, "");
		}
		if (esil) {
			esil->stack_addr = addr;
			esil->stack_size = size;
		}
		initialize_stack (core, addr, size);
		return;
	}

	if (!*input) {
		char *fi = sdb_get(core->sdb, "aeim.fd", 0);
		if (fi) {
			// Close the fd associated with the aeim stack
			ut64 fd = sdb_atoi (fi);
			(void)r_io_fd_close (core->io, fd);
		}
	}
	size = r_config_get_i (core->config, "esil.stack.size");
	addr = r_config_get_i (core->config, "esil.stack.addr");

	{
		RIOMap *map = r_io_map_get (core->io, addr);
		if (map) {
			addr = UT64_MAX;
		}
	}

	if (addr == UT64_MAX) {
		const ut64 align = 0x10000000;
		addr = r_io_map_next_available (core->io, core->offset, size, align);
	}
	patt = r_config_get (core->config, "esil.stack.pattern");
	p = strncpy (nomalloc, input, 255);
	if ((p = strchr (p, ' '))) {
		while (*p == ' ') p++;
		addr = r_num_math (core->num, p);
		if ((p = strchr (p, ' '))) {
			while (*p == ' ') p++;
			size = (ut32)r_num_math (core->num, p);
			if (size < 1) {
				size = 0xf0000;
			}
			if ((p = strchr (p, ' '))) {
				while (*p == ' ') {
					p++;
				}
				snprintf (name, sizeof (name), "mem.%s", p);
			} else {
				snprintf (name, sizeof (name), "mem.0x%" PFMT64x "_0x%x", addr, size);
			}
		} else {
			snprintf (name, sizeof (name), "mem.0x%" PFMT64x "_0x%x", addr, size);
		}
	} else {
		snprintf (name, sizeof (name), "mem.0x%" PFMT64x "_0x%x", addr, size);
	}
	if (*input == '-') {
		if (esil->stack_fd > 2) {	//0, 1, 2 are reserved for stdio/stderr
			r_io_fd_close (core->io, esil->stack_fd);
			// no need to kill the maps, r_io_map_cleanup does that for us in the close
			esil->stack_fd = 0;
		} else {
			eprintf ("Cannot deinitialize %s\n", name);
		}
		r_flag_unset_name (core->flags, name);
		r_flag_unset_name (core->flags, "aeim.stack");
		sdb_unset(core->sdb, "aeim.fd", 0);
		// eprintf ("Deinitialized %s\n", name);
		return;
	}

	snprintf (uri, sizeof (uri), "malloc://%d", (int)size);
	esil->stack_fd = r_io_fd_open (core->io, uri, R_PERM_RW, 0);
	if (!(stack_map = r_io_map_add (core->io, esil->stack_fd, R_PERM_RW, 0LL, addr, size))) {
		r_io_fd_close (core->io, esil->stack_fd);
		eprintf ("Cannot create map for tha stack, fd %d got closed again\n", esil->stack_fd);
		esil->stack_fd = 0;
		return;
	}
	r_io_map_set_name (stack_map, name);
	// r_flag_set (core->flags, name, addr, size);	//why is this here?
	char val[128], *v;
	v = sdb_itoa (esil->stack_fd, val, 10);
	sdb_set(core->sdb, "aeim.fd", v, 0);

	r_config_set_i (core->config, "io.va", true);
	if (patt && *patt) {
		switch (*patt) {
		case '0':
			// do nothing
			break;
		case 'd':
			r_core_cmdf (core, "wopD %d @ 0x%"PFMT64x, size, addr);
			break;
		case 'i':
			r_core_cmdf (core, "woe 0 255 1 @ 0x%"PFMT64x"!%d",addr, size);
			break;
		case 'w':
			r_core_cmdf (core, "woe 0 0xffff 1 4 @ 0x%"PFMT64x"!%d",addr, size);
			break;
		}
	}
	// SP
	sp = r_reg_get_name (core->dbg->reg, R_REG_NAME_SP);
	if (sp) {
		r_debug_reg_set (core->dbg, sp, addr + (size / 2));
	}
	// BP
	bp = r_reg_get_name (core->dbg->reg, R_REG_NAME_BP);
	if (bp) {
		r_debug_reg_set (core->dbg, bp, addr + (size / 2));
	}
	// PC
	pc = r_reg_get_name (core->dbg->reg, R_REG_NAME_PC);
	if (pc) {
		r_debug_reg_set (core->dbg, pc, curoff);
	}
	r_core_cmd0 (core, ".ar*");
	if (esil) {
		esil->stack_addr = addr;
		esil->stack_size = size;
	}
	initialize_stack (core, addr, size);
	r_core_seek (core, curoff, 0);
}