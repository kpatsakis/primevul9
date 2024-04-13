RZ_API void rz_bin_dwarf_debug_abbrev_free(RzBinDwarfDebugAbbrev *da) {
	size_t i;
	if (!da) {
		return;
	}
	for (i = 0; i < da->count; i++) {
		RZ_FREE(da->decls[i].defs);
	}
	RZ_FREE(da->decls);
	free(da);
}