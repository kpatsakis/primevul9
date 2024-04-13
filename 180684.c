static inline int pfn_level_offset(unsigned long pfn, int level)
{
	return (pfn >> level_to_offset_bits(level)) & LEVEL_MASK;
}