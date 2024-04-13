static void type_cmd(RCore *core, const char *input) {
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, -1);
	if (!fcn && *input != '?') {
		eprintf ("cant find function here\n");
		return;
	}
	ut64 seek;
	r_cons_break_push (NULL, NULL);
	switch (*input) {
	case '\0': // "aft"
		seek = core->offset;
		r_anal_esil_set_pc (core->anal->esil, fcn? fcn->addr: core->offset);
		r_core_anal_type_match (core, fcn);
		r_core_seek (core, seek, true);
		break;
	case '?':
		r_core_cmd_help (core, help_msg_aft);
		break;
	}
	r_cons_break_pop ();
}