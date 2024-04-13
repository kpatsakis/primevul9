int kvm_mmu_load(struct kvm_vcpu *vcpu)
{
	int r;

	r = mmu_topup_memory_caches(vcpu, !vcpu->arch.mmu->direct_map);
	if (r)
		goto out;
	r = mmu_alloc_special_roots(vcpu);
	if (r)
		goto out;
	if (vcpu->arch.mmu->direct_map)
		r = mmu_alloc_direct_roots(vcpu);
	else
		r = mmu_alloc_shadow_roots(vcpu);
	if (r)
		goto out;

	kvm_mmu_sync_roots(vcpu);

	kvm_mmu_load_pgd(vcpu);

	/*
	 * Flush any TLB entries for the new root, the provenance of the root
	 * is unknown.  Even if KVM ensures there are no stale TLB entries
	 * for a freed root, in theory another hypervisor could have left
	 * stale entries.  Flushing on alloc also allows KVM to skip the TLB
	 * flush when freeing a root (see kvm_tdp_mmu_put_root()).
	 */
	static_call(kvm_x86_flush_tlb_current)(vcpu);
out:
	return r;
}