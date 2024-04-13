static bool isSkippable(RBinSymbol *s) {
	if (s && s->name && s->bind) {
		if (r_str_startswith (s->name, "radr://")) {
			return true;
		}
		if (!strcmp (s->name, "__mh_execute_header")) {
			return true;
		}
		if (!strcmp (s->bind, "NONE")) {
			if (s->is_imported && s->libname && strstr(s->libname, ".dll")) {
				return true;
			}
		}
	}
	return false;
}