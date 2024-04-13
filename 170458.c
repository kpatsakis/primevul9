char *MACH0_(section_type_to_string)(ut64 type) {
	switch (type) {
	case S_REGULAR:
		return rz_str_new("REGULAR");
	case S_ZEROFILL:
		return rz_str_new("ZEROFILL");
	case S_CSTRING_LITERALS:
		return rz_str_new("CSTRING_LITERALS");
	case S_4BYTE_LITERALS:
		return rz_str_new("4BYTE_LITERALS");
	case S_LITERAL_POINTERS:
		return rz_str_new("LITERAL_POINTERS");
	case S_NON_LAZY_SYMBOL_POINTERS:
		return rz_str_new("NON_LAZY_SYMBOL_POINTERS");
	case S_LAZY_SYMBOL_POINTERS:
		return rz_str_new("LAZY_SYMBOL_POINTERS");
	case S_SYMBOL_STUBS:
		return rz_str_new("SYMBOL_STUBS");
	case S_MOD_INIT_FUNC_POINTERS:
		return rz_str_new("MOD_INIT_FUNC_POINTERS");
	case S_MOD_TERM_FUNC_POINTERS:
		return rz_str_new("MOD_TERM_FUNC_POINTERS");
	case S_COALESCED:
		return rz_str_new("COALESCED");
	case S_GB_ZEROFILL:
		return rz_str_new("GB_ZEROFILL");
	default:
		return rz_str_newf("0x%" PFMT64x, type);
	}
}