void kvm_mmu_hugepage_adjust(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault)
{
	struct kvm_memory_slot *slot = fault->slot;
	kvm_pfn_t mask;

	fault->huge_page_disallowed = fault->exec && fault->nx_huge_page_workaround_enabled;

	if (unlikely(fault->max_level == PG_LEVEL_4K))
		return;

	if (is_error_noslot_pfn(fault->pfn) || kvm_is_reserved_pfn(fault->pfn))
		return;

	if (kvm_slot_dirty_track_enabled(slot))
		return;

	/*
	 * Enforce the iTLB multihit workaround after capturing the requested
	 * level, which will be used to do precise, accurate accounting.
	 */
	fault->req_level = kvm_mmu_max_mapping_level(vcpu->kvm, slot,
						     fault->gfn, fault->pfn,
						     fault->max_level);
	if (fault->req_level == PG_LEVEL_4K || fault->huge_page_disallowed)
		return;

	/*
	 * mmu_notifier_retry() was successful and mmu_lock is held, so
	 * the pmd can't be split from under us.
	 */
	fault->goal_level = fault->req_level;
	mask = KVM_PAGES_PER_HPAGE(fault->goal_level) - 1;
	VM_BUG_ON((fault->gfn & mask) != (fault->pfn & mask));
	fault->pfn &= ~mask;
}