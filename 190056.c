static void __set_spte(u64 *sptep, u64 spte)
{
	union split_spte *ssptep, sspte;

	ssptep = (union split_spte *)sptep;
	sspte = (union split_spte)spte;

	ssptep->spte_high = sspte.spte_high;

	/*
	 * If we map the spte from nonpresent to present, We should store
	 * the high bits firstly, then set present bit, so cpu can not
	 * fetch this spte while we are setting the spte.
	 */
	smp_wmb();

	WRITE_ONCE(ssptep->spte_low, sspte.spte_low);
}