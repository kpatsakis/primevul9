static void count_spte_clear(u64 *sptep, u64 spte)
{
	struct kvm_mmu_page *sp =  sptep_to_sp(sptep);

	if (is_shadow_present_pte(spte))
		return;

	/* Ensure the spte is completely set before we increase the count */
	smp_wmb();
	sp->clear_spte_count++;
}