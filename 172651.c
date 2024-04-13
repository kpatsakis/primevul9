static inline int scrub_calc_parity_bitmap_len(int nsectors)
{
	return DIV_ROUND_UP(nsectors, BITS_PER_LONG) * sizeof(long);
}