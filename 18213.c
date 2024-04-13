static bool init_debug_info(RzBinDwarfDebugInfo *inf) {
	inf->comp_units = RZ_NEWS0(RzBinDwarfCompUnit, DEBUG_INFO_CAPACITY);
	if (!inf->comp_units) {
		return false;
	}
	inf->lookup_table = ht_up_new0();
	if (!inf->lookup_table) {
		goto wurzelbert_comp_units;
	}
	inf->line_info_offset_comp_dir = ht_up_new(NULL, free_ht_comp_dir, NULL);
	if (!inf->line_info_offset_comp_dir) {
		goto wurzelbert_lookup_table;
	}
	inf->capacity = DEBUG_INFO_CAPACITY;
	inf->count = 0;
	return true;
wurzelbert_lookup_table:
	ht_up_free(inf->lookup_table);
wurzelbert_comp_units:
	free(inf->comp_units);
	return false;
}