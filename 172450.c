static inline void scrub_parity_mark_sectors_data(struct scrub_parity *sparity,
						  u64 start, u64 len)
{
	__scrub_mark_bitmap(sparity, sparity->dbitmap, start, len);
}