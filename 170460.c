bool MACH0_(has_nx)(struct MACH0_(obj_t) * bin) {
	return (bin && bin->hdr.filetype == MH_EXECUTE &&
		bin->hdr.flags & MH_NO_HEAP_EXECUTION);
}