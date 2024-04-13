static u64 load_latency_data(u64 status)
{
	union intel_x86_pebs_dse dse;
	u64 val;

	dse.val = status;

	/*
	 * use the mapping table for bit 0-3
	 */
	val = pebs_data_source[dse.ld_dse];

	/*
	 * Nehalem models do not support TLB, Lock infos
	 */
	if (x86_pmu.pebs_no_tlb) {
		val |= P(TLB, NA) | P(LOCK, NA);
		return val;
	}
	/*
	 * bit 4: TLB access
	 * 0 = did not miss 2nd level TLB
	 * 1 = missed 2nd level TLB
	 */
	if (dse.ld_stlb_miss)
		val |= P(TLB, MISS) | P(TLB, L2);
	else
		val |= P(TLB, HIT) | P(TLB, L1) | P(TLB, L2);

	/*
	 * bit 5: locked prefix
	 */
	if (dse.ld_locked)
		val |= P(LOCK, LOCKED);

	/*
	 * Ice Lake and earlier models do not support block infos.
	 */
	if (!x86_pmu.pebs_block) {
		val |= P(BLK, NA);
		return val;
	}
	/*
	 * bit 6: load was blocked since its data could not be forwarded
	 *        from a preceding store
	 */
	if (dse.ld_data_blk)
		val |= P(BLK, DATA);

	/*
	 * bit 7: load was blocked due to potential address conflict with
	 *        a preceding store
	 */
	if (dse.ld_addr_blk)
		val |= P(BLK, ADDR);

	if (!dse.ld_data_blk && !dse.ld_addr_blk)
		val |= P(BLK, NA);

	return val;
}