RZ_API void rz_bin_dwarf_arange_set_free(RzBinDwarfARangeSet *set) {
	if (!set) {
		return;
	}
	free(set->aranges);
	free(set);
}