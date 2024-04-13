R_API void r_core_anal_undefine(RCore *core, ut64 off) {
	// very slow
	// RAnalFunction *f = r_anal_get_fcn_in (core->anal, off, -1);
	RAnalFunction *f = r_anal_get_function_at (core->anal, off);
	if (f) {
		if (!strncmp (f->name, "fcn.", 4)) {
			r_flag_unset_name (core->flags, f->name);
		}
		r_meta_del (core->anal, R_META_TYPE_ANY, r_anal_function_min_addr (f), r_anal_function_linear_size (f));
		r_anal_function_del (core->anal, off);
	}
	//r_anal_function_del_locs (core->anal, off);
	r_anal_delete_block_at (core->anal, off);
	char *abcmd = r_str_newf ("ab-0x%"PFMT64x, off);
	cmd_anal_fcn (core, abcmd);
	free (abcmd);
}