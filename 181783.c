static void r_anal_aefa(RCore *core, const char *arg) {
	ut64 to = r_num_math (core->num, arg);
	ut64 at, from = core->offset;
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, to, -1);
	if (!from || from == UT64_MAX) {
		if (fcn) {
			from = fcn->addr;
		} else {
			eprintf ("Usage: aefa [from] # if no from address is given, uses fcn.addr\n");
			return;
		}
	}
	eprintf ("Emulate from 0x%08"PFMT64x" to 0x%08"PFMT64x"\n", from, to);
	eprintf ("Resolve call args for 0x%08"PFMT64x"\n", to);

	// emulate
	// XXX do not use commands, here, just use the api
	r_core_cmd0 (core, "aeim"); // XXX
	ut64 off = core->offset;
	for (at = from; at < to ; at++) {
		r_core_cmdf (core, "aepc 0x%08"PFMT64x, at);
		r_core_cmd0 (core, "aeso");
		r_core_seek (core, at, 1);
		int delta = r_num_get (core->num, "$l");
		if (delta < 1) {
			break;
		}
		at += delta - 1;
	}
	r_core_seek (core, off, 1);

	// the logic of identifying args by function types and
	// show json format and arg name goes into arA
	r_core_cmd0 (core, "arA");
#if 0
	// get results
	const char *fcn_type = r_type_func_ret (core->anal->sdb_types, fcn->name);
	const char *key = resolve_fcn_name (core->anal, fcn->name);
	RList *list = r_core_get_func_args (core, key);
	if (!r_list_empty (list)) {
		eprintf ("HAS signature\n");
	}
	int i, nargs = 3; // r_type_func_args_count (core->anal->sdb_types, fcn->name);
	if (nargs > 0) {
		int i;
		eprintf ("NARGS %d (%s)\n", nargs, key);
		for (i = 0; i < nargs; i++) {
			ut64 v = r_debug_arg_get (core->dbg, R_ANAL_CC_TYPE_STDCALL, i);
			eprintf ("arg: 0x%08"PFMT64x"\n", v);
		}
	}
#endif
}