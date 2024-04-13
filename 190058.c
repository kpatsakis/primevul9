static u64 __update_clear_spte_slow(u64 *sptep, u64 spte)
{
	union split_spte *ssptep, sspte, orig;

	ssptep = (union split_spte *)sptep;
	sspte = (union split_spte)spte;

	/* xchg acts as a barrier before the setting of the high bits */
	orig.spte_low = xchg(&ssptep->spte_low, sspte.spte_low);
	orig.spte_high = ssptep->spte_high;
	ssptep->spte_high = sspte.spte_high;
	count_spte_clear(sptep, spte);

	return orig.spte;
}