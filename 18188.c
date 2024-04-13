static int init_debug_abbrev(RzBinDwarfDebugAbbrev *da) {
	if (!da) {
		return -EINVAL;
	}
	da->decls = calloc(sizeof(RzBinDwarfAbbrevDecl), DEBUG_ABBREV_CAP);
	if (!da->decls) {
		return -ENOMEM;
	}
	da->capacity = DEBUG_ABBREV_CAP;
	da->count = 0;

	return 0;
}