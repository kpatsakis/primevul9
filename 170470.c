RZ_API ut64 MACH0_(paddr_to_vaddr)(struct MACH0_(obj_t) * bin, ut64 offset) {
	if (bin->segs) {
		size_t i;
		for (i = 0; i < bin->nsegs; i++) {
			ut64 segment_base = (ut64)bin->segs[i].fileoff;
			ut64 segment_size = (ut64)bin->segs[i].filesize;
			if (offset >= segment_base && offset < segment_base + segment_size) {
				return bin->segs[i].vmaddr + (offset - segment_base);
			}
		}
	}
	return 0;
}