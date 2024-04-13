static char * getFunctionName (RCore *core, ut64 off, const char *name, bool prefix) {
	const char *fcnpfx = "";
	if (prefix) {
		if (fcnNeedsPrefix (name) && (!fcnpfx || !*fcnpfx)) {
			fcnpfx = "fcn";
		} else {
			fcnpfx = r_config_get (core->config, "anal.fcnprefix");
		}
	}
	if (r_reg_get (core->anal->reg, name, -1)) {
		return r_str_newf ("%s.%08"PFMT64x, "fcn", off);
	}
	return strdup (name); // r_str_newf ("%s%s%s", fcnpfx, *fcnpfx? ".": "", name);
}