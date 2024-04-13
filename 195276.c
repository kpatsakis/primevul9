static u64 store_latency_data(u64 status)
{
	union intel_x86_pebs_dse dse;
	u64 val;

	dse.val = status;

	/*
	 * use the mapping table for bit 0-3
	 */
	val = pebs_data_source[dse.st_lat_dse];

	/*
	 * bit 4: TLB access
	 * 0 = did not miss 2nd level TLB
	 * 1 = missed 2nd level TLB
	 */
	if (dse.st_lat_stlb_miss)
		val |= P(TLB, MISS) | P(TLB, L2);
	else
		val |= P(TLB, HIT) | P(TLB, L1) | P(TLB, L2);

	/*
	 * bit 5: locked prefix
	 */
	if (dse.st_lat_locked)
		val |= P(LOCK, LOCKED);

	val |= P(BLK, NA);

	return val;
}