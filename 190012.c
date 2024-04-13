static void walk_shadow_page_lockless_begin(struct kvm_vcpu *vcpu)
{
	if (is_tdp_mmu(vcpu->arch.mmu)) {
		kvm_tdp_mmu_walk_lockless_begin();
	} else {
		/*
		 * Prevent page table teardown by making any free-er wait during
		 * kvm_flush_remote_tlbs() IPI to all active vcpus.
		 */
		local_irq_disable();

		/*
		 * Make sure a following spte read is not reordered ahead of the write
		 * to vcpu->mode.
		 */
		smp_store_mb(vcpu->mode, READING_SHADOW_PAGE_TABLES);
	}
}