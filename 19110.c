static void set_fcn_name_from_flag(RAnalFunction *fcn, RFlagItem *f, const char *fcnpfx) {
	bool nameChanged = false;
	if (f && f->name) {
		if (!strncmp (fcn->name, "loc.", 4) || !strncmp (fcn->name, "fcn.", 4)) {
			r_anal_function_rename (fcn, f->name);
			nameChanged = true;
		} else if (strncmp (f->name, "sect", 4)) {
			r_anal_function_rename (fcn, f->name);
			nameChanged = true;
		}
	}
	if (!nameChanged) {
		char *nn = r_str_newf ("%s.%08" PFMT64x, fcnpfx, fcn->addr);
		r_anal_function_rename (fcn, nn);
		free (nn);
	}
}