static bool kvm_test_age_rmapp(struct kvm *kvm, struct kvm_rmap_head *rmap_head,
			       struct kvm_memory_slot *slot, gfn_t gfn,
			       int level, pte_t unused)
{
	u64 *sptep;
	struct rmap_iterator iter;

	for_each_rmap_spte(rmap_head, &iter, sptep)
		if (is_accessed_spte(*sptep))
			return true;
	return false;
}