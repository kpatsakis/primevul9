bool MACH0_(is_pie)(struct MACH0_(obj_t) * bin) {
	return (bin && bin->hdr.filetype == MH_EXECUTE && bin->hdr.flags & MH_PIE);
}