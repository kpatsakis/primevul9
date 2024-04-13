static inline unsigned long level_size(int level)
{
	return 1UL << level_to_offset_bits(level);
}