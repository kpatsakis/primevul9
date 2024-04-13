static bool is_page_fault_stale(struct kvm_vcpu *vcpu,
				struct kvm_page_fault *fault, int mmu_seq)
{
	struct kvm_mmu_page *sp = to_shadow_page(vcpu->arch.mmu->root.hpa);

	/* Special roots, e.g. pae_root, are not backed by shadow pages. */
	if (sp && is_obsolete_sp(vcpu->kvm, sp))
		return true;

	/*
	 * Roots without an associated shadow page are considered invalid if
	 * there is a pending request to free obsolete roots.  The request is
	 * only a hint that the current root _may_ be obsolete and needs to be
	 * reloaded, e.g. if the guest frees a PGD that KVM is tracking as a
	 * previous root, then __kvm_mmu_prepare_zap_page() signals all vCPUs
	 * to reload even if no vCPU is actively using the root.
	 */
	if (!sp && kvm_test_request(KVM_REQ_MMU_FREE_OBSOLETE_ROOTS, vcpu))
		return true;

	return fault->slot &&
	       mmu_notifier_retry_hva(vcpu->kvm, mmu_seq, fault->hva);
}