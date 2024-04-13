static bool __setFunctionName(RCore *core, ut64 addr, const char *_name, bool prefix) {
	r_return_val_if_fail (core && _name, false);
	_name = r_str_trim_head_ro (_name);
	char *name = getFunctionName (core, addr, _name, prefix);
	// RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, addr, R_ANAL_FCN_TYPE_ANY);
	RAnalFunction *fcn = r_anal_get_function_at (core->anal, addr);
	if (fcn) {
		RFlagItem *flag = r_flag_get (core->flags, fcn->name);
		if (flag && flag->space && strcmp (flag->space->name, R_FLAGS_FS_FUNCTIONS) == 0) {
			// Only flags in the functions fs should be renamed, e.g. we don't want to rename symbol flags.
			r_flag_rename (core->flags, flag, name);
		} else {
			// No flag or not specific to the function, create a new one.
			r_flag_space_push (core->flags, R_FLAGS_FS_FUNCTIONS);
			r_flag_set (core->flags, name, fcn->addr, r_anal_function_size_from_entry (fcn));
			r_flag_space_pop (core->flags);
		}
		r_anal_function_rename (fcn, name);
		if (core->anal->cb.on_fcn_rename) {
			core->anal->cb.on_fcn_rename (core->anal, core->anal->user, fcn, name);
		}
		free (name);
		return true;
	}
	free (name);
	return false;
}