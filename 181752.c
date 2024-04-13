static void show_reg_args(RCore *core, int nargs, RStrBuf *sb) {
	int i;
	char regname[8];
	if (nargs < 0) {
		nargs = 4; // default args if not defined
	}
	for (i = 0; i < nargs; i++) {
		snprintf (regname, sizeof (regname), "A%d", i);
		ut64 v = r_reg_getv (core->anal->reg, regname);
		if (sb) {
			r_strbuf_appendf (sb, "%s0x%08"PFMT64x, i?", ":"", v);
		} else {
			r_cons_printf ("A%d 0x%08"PFMT64x"\n", i, v);
		}
	}
}