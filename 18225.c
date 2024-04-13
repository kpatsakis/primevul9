static int expand_debug_abbrev(RzBinDwarfDebugAbbrev *da) {
	RzBinDwarfAbbrevDecl *tmp;

	if (!da || da->capacity == 0 || da->capacity != da->count) {
		return -EINVAL;
	}

	tmp = (RzBinDwarfAbbrevDecl *)realloc(da->decls,
		da->capacity * 2 * sizeof(RzBinDwarfAbbrevDecl));

	if (!tmp) {
		return -ENOMEM;
	}
	memset((ut8 *)tmp + da->capacity * sizeof(RzBinDwarfAbbrevDecl),
		0, da->capacity * sizeof(RzBinDwarfAbbrevDecl));

	da->decls = tmp;
	da->capacity *= 2;

	return 0;
}