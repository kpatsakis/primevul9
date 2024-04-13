int MACH0_(get_bits)(struct MACH0_(obj_t) * bin) {
	if (bin) {
		int bits = MACH0_(get_bits_from_hdr)(&bin->hdr);
		if (bin->hdr.cputype == CPU_TYPE_ARM && bin->entry & 1) {
			return 16;
		}
		return bits;
	}
	return 32;
}