static inline unsigned long lvl_to_nr_pages(unsigned int lvl)
{
	return  1 << min_t(int, (lvl - 1) * LEVEL_STRIDE, MAX_AGAW_PFN_WIDTH);
}