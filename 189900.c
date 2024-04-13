static u64 __get_spte_lockless(u64 *sptep)
{
	return READ_ONCE(*sptep);
}