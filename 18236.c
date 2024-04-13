RZ_API void rz_bin_dwarf_line_header_free_file_cache(const RzBinDwarfLineHeader *hdr, RzBinDwarfLineFileCache fnc) {
	if (!fnc) {
		return;
	}
	for (size_t i = 0; i < hdr->file_names_count; i++) {
		free(fnc[i]);
	}
	free(fnc);
}