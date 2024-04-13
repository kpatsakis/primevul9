static bool __rmap_clear_dirty(struct kvm *kvm, struct kvm_rmap_head *rmap_head,
			       const struct kvm_memory_slot *slot)
{
	u64 *sptep;
	struct rmap_iterator iter;
	bool flush = false;

	for_each_rmap_spte(rmap_head, &iter, sptep)
		if (spte_ad_need_write_protect(*sptep))
			flush |= spte_wrprot_for_clear_dirty(sptep);
		else
			flush |= spte_clear_dirty(sptep);

	return flush;
}