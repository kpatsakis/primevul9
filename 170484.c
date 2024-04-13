RZ_API ut64 MACH0_(vaddr_to_paddr)(struct MACH0_(obj_t) * bin, ut64 addr) {
	if (bin->segs) {
		size_t i;
		for (i = 0; i < bin->nsegs; i++) {
			const ut64 segment_base = (ut64)bin->segs[i].vmaddr;
			const ut64 segment_size = (ut64)bin->segs[i].vmsize;
			if (addr >= segment_base && addr < segment_base + segment_size) {
				return bin->segs[i].fileoff + (addr - segment_base);
			}
		}
	}
	return 0;
}