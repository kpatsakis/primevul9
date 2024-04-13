RZ_API const char *rz_bin_dwarf_get_attr_form_name(ut64 form_code) {
	if (form_code < DW_FORM_addr || form_code > DW_FORM_addrx4) {
		return NULL;
	}
	return dwarf_attr_form_encodings[form_code];
}