static int svm_interrupt_allowed(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	struct vmcb *vmcb = svm->vmcb;

	if (!gif_set(svm) ||
	     (vmcb->control.int_state & SVM_INTERRUPT_SHADOW_MASK))
		return 0;

	if (is_guest_mode(vcpu) && (svm->vcpu.arch.hflags & HF_VINTR_MASK))
		return !!(svm->vcpu.arch.hflags & HF_HIF_MASK);
	else
		return !!(kvm_get_rflags(vcpu) & X86_EFLAGS_IF);
}