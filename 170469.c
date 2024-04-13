char *MACH0_(get_cpusubtype)(struct MACH0_(obj_t) * bin) {
	return bin ? MACH0_(get_cpusubtype_from_hdr)(&bin->hdr) : strdup("Unknown");
}