char *MACH0_(get_cpusubtype_from_hdr)(struct MACH0_(mach_header) * hdr) {
	rz_return_val_if_fail(hdr, NULL);
	return strdup(cpusubtype_tostring(hdr->cputype, hdr->cpusubtype));
}