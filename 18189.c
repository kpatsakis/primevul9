RZ_API const char *rz_bin_dwarf_get_lang_name(ut64 lang) {
	if (lang >= RZ_ARRAY_SIZE(dwarf_langs)) {
		return NULL;
	}
	return dwarf_langs[lang];
}