static void flag_every_function(RCore *core) {
	RListIter *iter;
	RAnalFunction *fcn;
	r_flag_space_push (core->flags, R_FLAGS_FS_FUNCTIONS);
	r_list_foreach (core->anal->fcns, iter, fcn) {
		r_flag_set (core->flags, fcn->name,
			fcn->addr, r_anal_function_size_from_entry (fcn));
	}
	r_flag_space_pop (core->flags);
}