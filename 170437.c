bool MACH0_(is_big_endian)(struct MACH0_(obj_t) * bin) {
	if (bin) {
		const int cpu = bin->hdr.cputype;
		return cpu == CPU_TYPE_POWERPC || cpu == CPU_TYPE_POWERPC64;
	}
	return false;
}