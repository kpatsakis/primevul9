void svm_flush_tlb(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	/*
	 * Flush only the current ASID even if the TLB flush was invoked via
	 * kvm_flush_remote_tlbs().  Although flushing remote TLBs requires all
	 * ASIDs to be flushed, KVM uses a single ASID for L1 and L2, and
	 * unconditionally does a TLB flush on both nested VM-Enter and nested
	 * VM-Exit (via kvm_mmu_reset_context()).
	 */
	if (static_cpu_has(X86_FEATURE_FLUSHBYASID))
		svm->vmcb->control.tlb_ctl = TLB_CONTROL_FLUSH_ASID;
	else
		svm->asid_generation--;
}