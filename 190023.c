void kvm_mmu_new_pgd(struct kvm_vcpu *vcpu, gpa_t new_pgd)
{
	struct kvm_mmu *mmu = vcpu->arch.mmu;
	union kvm_mmu_page_role new_role = mmu->mmu_role.base;

	if (!fast_pgd_switch(vcpu->kvm, mmu, new_pgd, new_role)) {
		/* kvm_mmu_ensure_valid_pgd will set up a new root.  */
		return;
	}

	/*
	 * It's possible that the cached previous root page is obsolete because
	 * of a change in the MMU generation number. However, changing the
	 * generation number is accompanied by KVM_REQ_MMU_FREE_OBSOLETE_ROOTS,
	 * which will free the root set here and allocate a new one.
	 */
	kvm_make_request(KVM_REQ_LOAD_MMU_PGD, vcpu);

	if (force_flush_and_sync_on_reuse) {
		kvm_make_request(KVM_REQ_MMU_SYNC, vcpu);
		kvm_make_request(KVM_REQ_TLB_FLUSH_CURRENT, vcpu);
	}

	/*
	 * The last MMIO access's GVA and GPA are cached in the VCPU. When
	 * switching to a new CR3, that GVA->GPA mapping may no longer be
	 * valid. So clear any cached MMIO info even when we don't need to sync
	 * the shadow page tables.
	 */
	vcpu_clear_mmio_info(vcpu, MMIO_GVA_ANY);

	/*
	 * If this is a direct root page, it doesn't have a write flooding
	 * count. Otherwise, clear the write flooding count.
	 */
	if (!new_role.direct)
		__clear_sp_write_flooding_count(
				to_shadow_page(vcpu->arch.mmu->root.hpa));
}