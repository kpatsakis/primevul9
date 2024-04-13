static void __update_clear_spte_fast(u64 *sptep, u64 spte)
{
	WRITE_ONCE(*sptep, spte);
}