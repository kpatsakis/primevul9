static void walk_shadow_page_lockless_end(struct kvm_vcpu *vcpu)
{
	if (is_tdp_mmu(vcpu->arch.mmu)) {
		kvm_tdp_mmu_walk_lockless_end();
	} else {
		/*
		 * Make sure the write to vcpu->mode is not reordered in front of
		 * reads to sptes.  If it does, kvm_mmu_commit_zap_page() can see us
		 * OUTSIDE_GUEST_MODE and proceed to free the shadow page table.
		 */
		smp_store_release(&vcpu->mode, OUTSIDE_GUEST_MODE);
		local_irq_enable();
	}
}