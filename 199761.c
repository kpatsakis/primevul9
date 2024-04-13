static inline void __snmp6_fill_statsdev(u64 *stats, atomic_long_t *mib,
				      int items, int bytes)
{
	int i;
	int pad = bytes - sizeof(u64) * items;
	BUG_ON(pad < 0);

	/* Use put_unaligned() because stats may not be aligned for u64. */
	put_unaligned(items, &stats[0]);
	for (i = 1; i < items; i++)
		put_unaligned(atomic_long_read(&mib[i]), &stats[i]);

	memset(&stats[items], 0, pad);
}