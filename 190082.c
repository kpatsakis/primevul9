static bool kvm_set_pte_rmapp(struct kvm *kvm, struct kvm_rmap_head *rmap_head,
			      struct kvm_memory_slot *slot, gfn_t gfn, int level,
			      pte_t pte)
{
	u64 *sptep;
	struct rmap_iterator iter;
	bool need_flush = false;
	u64 new_spte;
	kvm_pfn_t new_pfn;

	WARN_ON(pte_huge(pte));
	new_pfn = pte_pfn(pte);

restart:
	for_each_rmap_spte(rmap_head, &iter, sptep) {
		rmap_printk("spte %p %llx gfn %llx (%d)\n",
			    sptep, *sptep, gfn, level);

		need_flush = true;

		if (pte_write(pte)) {
			pte_list_remove(kvm, rmap_head, sptep);
			goto restart;
		} else {
			new_spte = kvm_mmu_changed_pte_notifier_make_spte(
					*sptep, new_pfn);

			mmu_spte_clear_track_bits(kvm, sptep);
			mmu_spte_set(sptep, new_spte);
		}
	}

	if (need_flush && kvm_available_flush_tlb_with_range()) {
		kvm_flush_remote_tlbs_with_address(kvm, gfn, 1);
		return false;
	}

	return need_flush;
}