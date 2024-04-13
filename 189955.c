static void mmu_spte_clear_no_track(u64 *sptep)
{
	__update_clear_spte_fast(sptep, 0ull);
}