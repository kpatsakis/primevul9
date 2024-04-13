static bool is_unknown_file(RCore *core) {
	if (core->bin->cur && core->bin->cur->o) {
		return (r_list_empty (core->bin->cur->o->sections));
	}
	return true;
}