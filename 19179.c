R_API char *r_core_anal_fcn_name(RCore *core, RAnalFunction *fcn) {
	bool demangle = r_config_get_i (core->config, "bin.demangle");
	const char *lang = demangle ? r_config_get (core->config, "bin.lang") : NULL;
	bool keep_lib = r_config_get_i (core->config, "bin.demangle.libs");
	char *name = strdup (r_str_get (fcn->name));
	if (demangle) {
		char *tmp = r_bin_demangle (core->bin->cur, lang, name, fcn->addr, keep_lib);
		if (tmp) {
			free (name);
			name = tmp;
		}
	}
	return name;
}