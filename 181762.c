static void afCc(RCore *core, const char *input) {
	ut64 addr;
	RAnalFunction *fcn;
	if (*input == ' ') {
		addr = r_num_math (core->num, input);
	} else {
		addr = core->offset;
	}
	if (addr == 0LL) {
		fcn = r_anal_fcn_find_name (core->anal, input + 3);
	} else {
		fcn = r_anal_get_fcn_in (core->anal, addr, R_ANAL_FCN_TYPE_NULL);
	}
	if (fcn) {
		ut32 totalCycles = r_anal_fcn_cost (core->anal, fcn);
		// FIXME: This defeats the purpose of the function, but afC is used in project files.
		// cf. canal.c
		r_cons_printf ("%d\n", totalCycles);
	} else {
		eprintf ("afCc: Cannot find function\n");
	}
}