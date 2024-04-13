static void nested_svm_transition_tlb_flush(struct kvm_vcpu *vcpu)
{
	/*
	 * TODO: optimize unconditional TLB flush/MMU sync.  A partial list of
	 * things to fix before this can be conditional:
	 *
	 *  - Flush TLBs for both L1 and L2 remote TLB flush
	 *  - Honor L1's request to flush an ASID on nested VMRUN
	 *  - Sync nested NPT MMU on VMRUN that flushes L2's ASID[*]
	 *  - Don't crush a pending TLB flush in vmcb02 on nested VMRUN
	 *  - Flush L1's ASID on KVM_REQ_TLB_FLUSH_GUEST
	 *
	 * [*] Unlike nested EPT, SVM's ASID management can invalidate nested
	 *     NPT guest-physical mappings on VMRUN.
	 */
	kvm_make_request(KVM_REQ_MMU_SYNC, vcpu);
	kvm_make_request(KVM_REQ_TLB_FLUSH_CURRENT, vcpu);
}