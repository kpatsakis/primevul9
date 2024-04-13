RZ_API const char *rz_bin_dwarf_get_tag_name(ut64 tag) {
	if (tag >= DW_TAG_LAST) {
		return NULL;
	}
	return dwarf_tag_name_encodings[tag];
}