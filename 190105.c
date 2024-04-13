static bool kvm_mmu_zap_collapsible_spte(struct kvm *kvm,
					 struct kvm_rmap_head *rmap_head,
					 const struct kvm_memory_slot *slot)
{
	u64 *sptep;
	struct rmap_iterator iter;
	int need_tlb_flush = 0;
	kvm_pfn_t pfn;
	struct kvm_mmu_page *sp;

restart:
	for_each_rmap_spte(rmap_head, &iter, sptep) {
		sp = sptep_to_sp(sptep);
		pfn = spte_to_pfn(*sptep);

		/*
		 * We cannot do huge page mapping for indirect shadow pages,
		 * which are found on the last rmap (level = 1) when not using
		 * tdp; such shadow pages are synced with the page table in
		 * the guest, and the guest page table is using 4K page size
		 * mapping if the indirect sp has level = 1.
		 */
		if (sp->role.direct && !kvm_is_reserved_pfn(pfn) &&
		    sp->role.level < kvm_mmu_max_mapping_level(kvm, slot, sp->gfn,
							       pfn, PG_LEVEL_NUM)) {
			pte_list_remove(kvm, rmap_head, sptep);

			if (kvm_available_flush_tlb_with_range())
				kvm_flush_remote_tlbs_with_address(kvm, sp->gfn,
					KVM_PAGES_PER_HPAGE(sp->role.level));
			else
				need_tlb_flush = 1;

			goto restart;
		}
	}

	return need_tlb_flush;
}