static bool kvm_age_rmapp(struct kvm *kvm, struct kvm_rmap_head *rmap_head,
			  struct kvm_memory_slot *slot, gfn_t gfn, int level,
			  pte_t unused)
{
	u64 *sptep;
	struct rmap_iterator iter;
	int young = 0;

	for_each_rmap_spte(rmap_head, &iter, sptep)
		young |= mmu_spte_age(sptep);

	return young;
}