static ut64 entry_to_vaddr(struct MACH0_(obj_t) * bin) {
	switch (bin->main_cmd.cmd) {
	case LC_MAIN:
		return bin->entry + bin->baddr;
	case LC_UNIXTHREAD:
	case LC_THREAD:
		return bin->entry;
	default:
		return 0;
	}
}