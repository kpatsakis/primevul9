static inline unsigned long align_to_level(unsigned long pfn, int level)
{
	return (pfn + level_size(level) - 1) & level_mask(level);
}