static bool init(struct MACH0_(obj_t) * mo) {
	if (!init_hdr(mo)) {
		return false;
	}
	if (!init_items(mo)) {
		Eprintf("Warning: Cannot initialize items\n");
	}
	mo->baddr = MACH0_(get_baddr)(mo);
	return true;
}