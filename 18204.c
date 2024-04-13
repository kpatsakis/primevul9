RZ_API const char *rz_bin_dwarf_get_unit_type_name(ut64 unit_type) {
	if (!unit_type || unit_type > DW_UT_split_type) {
		return NULL;
	}
	return dwarf_unit_types[unit_type];
}