static void cmd_address_info(RCore *core, const char *addrstr, int fmt) {
	ut64 addr, type;
	if (!addrstr || !*addrstr) {
		addr = core->offset;
	} else {
		addr = r_num_math (core->num, addrstr);
	}
	type = r_core_anal_address (core, addr);
	switch (fmt) {
	case 'j': {
		PJ *pj = pj_new ();
		if (!pj) {
			return;
		}
		pj_o (pj);
		if (type & R_ANAL_ADDR_TYPE_PROGRAM)
			pj_ks (pj, "program", "true");
		if (type & R_ANAL_ADDR_TYPE_LIBRARY)
			pj_ks (pj, "library", "true");
		if (type & R_ANAL_ADDR_TYPE_EXEC)
			pj_ks (pj, "exec", "true");
		if (type & R_ANAL_ADDR_TYPE_READ)
			pj_ks (pj, "read", "true");
		if (type & R_ANAL_ADDR_TYPE_WRITE)
			pj_ks (pj, "write", "true");
		if (type & R_ANAL_ADDR_TYPE_FLAG)
			pj_ks (pj, "flag", "true");
		if (type & R_ANAL_ADDR_TYPE_FUNC)
			pj_ks (pj, "func", "true");
		if (type & R_ANAL_ADDR_TYPE_STACK)
			pj_ks (pj, "stack", "true");
		if (type & R_ANAL_ADDR_TYPE_HEAP)
			pj_ks (pj, "heap", "true");
		if (type & R_ANAL_ADDR_TYPE_REG)
			pj_ks (pj, "reg", "true");
		if (type & R_ANAL_ADDR_TYPE_ASCII)
			pj_ks (pj, "ascii", "true");
		if (type & R_ANAL_ADDR_TYPE_SEQUENCE)
			pj_ks (pj, "sequence", "true");
		pj_end (pj);
		r_cons_println (pj_string (pj));
		pj_free (pj);
		}
		break;
	default:
		if (type & R_ANAL_ADDR_TYPE_PROGRAM)
			r_cons_printf ("program\n");
		if (type & R_ANAL_ADDR_TYPE_LIBRARY)
			r_cons_printf ("library\n");
		if (type & R_ANAL_ADDR_TYPE_EXEC)
			r_cons_printf ("exec\n");
		if (type & R_ANAL_ADDR_TYPE_READ)
			r_cons_printf ("read\n");
		if (type & R_ANAL_ADDR_TYPE_WRITE)
			r_cons_printf ("write\n");
		if (type & R_ANAL_ADDR_TYPE_FLAG)
			r_cons_printf ("flag\n");
		if (type & R_ANAL_ADDR_TYPE_FUNC)
			r_cons_printf ("func\n");
		if (type & R_ANAL_ADDR_TYPE_STACK)
			r_cons_printf ("stack\n");
		if (type & R_ANAL_ADDR_TYPE_HEAP)
			r_cons_printf ("heap\n");
		if (type & R_ANAL_ADDR_TYPE_REG)
			r_cons_printf ("reg\n");
		if (type & R_ANAL_ADDR_TYPE_ASCII)
			r_cons_printf ("ascii\n");
		if (type & R_ANAL_ADDR_TYPE_SEQUENCE)
			r_cons_printf ("sequence\n");
		break;
	}
}