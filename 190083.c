static u64 mmu_spte_get_lockless(u64 *sptep)
{
	return __get_spte_lockless(sptep);
}