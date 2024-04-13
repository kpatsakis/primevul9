static inline void __snmp6_fill_stats64(u64 *stats, void __percpu **mib,
				      int items, int bytes, size_t syncpoff)
{
	int i;
	int pad = bytes - sizeof(u64) * items;
	BUG_ON(pad < 0);

	/* Use put_unaligned() because stats may not be aligned for u64. */
	put_unaligned(items, &stats[0]);
	for (i = 1; i < items; i++)
		put_unaligned(snmp_fold_field64(mib, i, syncpoff), &stats[i]);

	memset(&stats[items], 0, pad);
}