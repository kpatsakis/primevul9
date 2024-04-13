static bool is_entry_flag(RFlagItem *f) {
	return f->space && !strcmp (f->space->name, R_FLAGS_FS_SYMBOLS) && r_str_startswith (f->name, "entry.");
}