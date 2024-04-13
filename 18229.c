RZ_API RzBinDwarfLineFileCache rz_bin_dwarf_line_header_new_file_cache(const RzBinDwarfLineHeader *hdr) {
	return RZ_NEWS0(char *, hdr->file_names_count);
}