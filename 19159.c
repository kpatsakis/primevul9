R_API void r_core_anal_autoname_all_fcns(RCore *core) {
	RListIter *it;
	RAnalFunction *fcn;

	r_list_foreach (core->anal->fcns, it, fcn) {
		if (!strncmp (fcn->name, "fcn.", 4) || !strncmp (fcn->name, "sym.func.", 9)) {
			RFlagItem *item = r_flag_get (core->flags, fcn->name);
			if (item) {
				char *name = anal_fcn_autoname (core, fcn, 0, 0);
				if (name) {
					r_flag_rename (core->flags, item, name);
					free (fcn->name);
					fcn->name = name;
				}
			} else {
				// there should always be a flag for a function
				r_warn_if_reached ();
			}
		}
	}
}