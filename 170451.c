const char *MACH0_(get_cputype)(struct MACH0_(obj_t) * bin) {
	return bin ? MACH0_(get_cputype_from_hdr)(&bin->hdr) : "unknown";
}