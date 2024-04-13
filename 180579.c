static inline unsigned long level_mask(int level)
{
	return -1UL << level_to_offset_bits(level);
}