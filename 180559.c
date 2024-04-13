static inline unsigned int level_to_offset_bits(int level)
{
	return (level - 1) * LEVEL_STRIDE;
}