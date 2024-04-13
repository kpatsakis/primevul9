static u64 __get_spte_lockless(u64 *sptep)
{
	struct kvm_mmu_page *sp =  sptep_to_sp(sptep);
	union split_spte spte, *orig = (union split_spte *)sptep;
	int count;

retry:
	count = sp->clear_spte_count;
	smp_rmb();

	spte.spte_low = orig->spte_low;
	smp_rmb();

	spte.spte_high = orig->spte_high;
	smp_rmb();

	if (unlikely(spte.spte_low != orig->spte_low ||
	      count != sp->clear_spte_count))
		goto retry;

	return spte.spte;
}