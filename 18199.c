RZ_API void rz_bin_dwarf_debug_info_free(RzBinDwarfDebugInfo *inf) {
	if (!inf) {
		return;
	}
	for (size_t i = 0; i < inf->count; i++) {
		free_comp_unit(&inf->comp_units[i]);
	}
	ht_up_free(inf->line_info_offset_comp_dir);
	ht_up_free(inf->lookup_table);
	free(inf->comp_units);
	free(inf);
}