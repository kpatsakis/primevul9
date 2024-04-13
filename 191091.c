static bool svm_apic_init_signal_blocked(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	/*
	 * TODO: Last condition latch INIT signals on vCPU when
	 * vCPU is in guest-mode and vmcb12 defines intercept on INIT.
	 * To properly emulate the INIT intercept, SVM should implement
	 * kvm_x86_ops.check_nested_events() and call nested_svm_vmexit()
	 * there if an INIT signal is pending.
	 */
	return !gif_set(svm) ||
		   (svm->vmcb->control.intercept & (1ULL << INTERCEPT_INIT));
}