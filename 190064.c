static u64 __update_clear_spte_slow(u64 *sptep, u64 spte)
{
	return xchg(sptep, spte);
}