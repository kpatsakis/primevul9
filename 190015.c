static void __set_spte(u64 *sptep, u64 spte)
{
	WRITE_ONCE(*sptep, spte);
}