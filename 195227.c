static u64 precise_store_data(u64 status)
{
	union intel_x86_pebs_dse dse;
	u64 val = P(OP, STORE) | P(SNOOP, NA) | P(LVL, L1) | P(TLB, L2);

	dse.val = status;

	/*
	 * bit 4: TLB access
	 * 1 = stored missed 2nd level TLB
	 *
	 * so it either hit the walker or the OS
	 * otherwise hit 2nd level TLB
	 */
	if (dse.st_stlb_miss)
		val |= P(TLB, MISS);
	else
		val |= P(TLB, HIT);

	/*
	 * bit 0: hit L1 data cache
	 * if not set, then all we know is that
	 * it missed L1D
	 */
	if (dse.st_l1d_hit)
		val |= P(LVL, HIT);
	else
		val |= P(LVL, MISS);

	/*
	 * bit 5: Locked prefix
	 */
	if (dse.st_locked)
		val |= P(LOCK, LOCKED);

	return val;
}