char *MACH0_(get_filetype)(struct MACH0_(obj_t) * bin) {
	return bin ? MACH0_(get_filetype_from_hdr)(&bin->hdr) : strdup("Unknown");
}