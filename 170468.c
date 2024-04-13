RzList *MACH0_(section_flag_to_rzlist)(ut64 flag) {
	RzList *flag_list = rz_list_new();
	if (flag & S_ATTR_PURE_INSTRUCTIONS) {
		rz_list_append(flag_list, "PURE_INSTRUCTIONS");
	}
	if (flag & S_ATTR_NO_TOC) {
		rz_list_append(flag_list, "NO_TOC");
	}
	if (flag & S_ATTR_SOME_INSTRUCTIONS) {
		rz_list_append(flag_list, "SOME_INSTRUCTIONS");
	}
	if (flag & S_ATTR_EXT_RELOC) {
		rz_list_append(flag_list, "EXT_RELOC");
	}
	if (flag & S_ATTR_SELF_MODIFYING_CODE) {
		rz_list_append(flag_list, "SELF_MODIFYING_CODE");
	}
	if (flag & S_ATTR_DEBUG) {
		rz_list_append(flag_list, "DEBUG");
	}
	if (flag & S_ATTR_LIVE_SUPPORT) {
		rz_list_append(flag_list, "LIVE_SUPPORT");
	}
	if (flag & S_ATTR_STRIP_STATIC_SYMS) {
		rz_list_append(flag_list, "STRIP_STATIC_SYMS");
	}
	if (flag & S_ATTR_NO_DEAD_STRIP) {
		rz_list_append(flag_list, "NO_DEAD_STRIP");
	}
	return flag_list;
}