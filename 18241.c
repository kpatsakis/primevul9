static inline RzBinDwarfLocRange *create_loc_range(ut64 start, ut64 end, RzBinDwarfBlock *block) {
	RzBinDwarfLocRange *range = RZ_NEW0(RzBinDwarfLocRange);
	if (range) {
		range->start = start;
		range->end = end;
		range->expression = block;
	}
	return range;
}