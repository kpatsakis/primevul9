static const char *get_full_file_path(const RzBinDwarfDebugInfo *info, const RzBinDwarfLineHeader *header,
	RZ_NULLABLE RzBinDwarfLineFileCache cache, ut64 file_index) {
	if (file_index >= header->file_names_count) {
		return NULL;
	}
	if (!cache) {
		return header->file_names[file_index].name;
	}
	if (!cache[file_index]) {
		cache[file_index] = rz_bin_dwarf_line_header_get_full_file_path(info, header, file_index);
	}
	return cache[file_index];
}