static int find_sym_flag(const void *a1, const void *a2) {
	const RFlagItem *f = (const RFlagItem *)a2;
	return f->space && !strcmp (f->space->name, R_FLAGS_FS_SYMBOLS)? 0: 1;
}