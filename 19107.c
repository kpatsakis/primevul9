static char *get_title(ut64 addr) {
	return r_str_newf ("0x%"PFMT64x, addr);
}