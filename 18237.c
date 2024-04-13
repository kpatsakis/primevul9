static void free_comp_unit(RzBinDwarfCompUnit *cu) {
	size_t i;
	if (!cu) {
		return;
	}
	for (i = 0; i < cu->count; i++) {
		if (cu->dies) {
			free_die(&cu->dies[i]);
		}
	}
	RZ_FREE(cu->dies);
}