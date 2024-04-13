RZ_API void rz_bin_dwarf_line_info_free(RzBinDwarfLineInfo *li) {
	if (!li) {
		return;
	}
	rz_list_free(li->units);
	rz_bin_source_line_info_free(li->lines);
	free(li);
}