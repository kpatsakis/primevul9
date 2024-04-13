static void function_rename(RFlag *flags, RAnalFunction *fcn) {
	const char *locname = "loc.";
	const size_t locsize = strlen (locname);
	char *fcnname = fcn->name;

	if (strncmp (fcn->name, locname, locsize) == 0) {
		const char *fcnpfx, *restofname;
		RFlagItem *f;

		fcn->type = R_ANAL_FCN_TYPE_FCN;
		fcnpfx = r_anal_functiontype_tostring (fcn->type);
		restofname = fcn->name + locsize;
		fcn->name = r_str_newf ("%s.%s", fcnpfx, restofname);

		f = r_flag_get_i (flags, fcn->addr);
		r_flag_rename (flags, f, fcn->name);

		free (fcnname);
	}
}