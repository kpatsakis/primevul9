static int svm_check_nested_events(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	bool block_nested_events =
		kvm_event_needs_reinjection(vcpu) || svm->nested.nested_run_pending;
	struct kvm_lapic *apic = vcpu->arch.apic;

	if (lapic_in_kernel(vcpu) &&
	    test_bit(KVM_APIC_INIT, &apic->pending_events)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_init(svm))
			return 0;
		nested_svm_simple_vmexit(svm, SVM_EXIT_INIT);
		return 0;
	}

	if (vcpu->arch.exception.pending) {
		/*
		 * Only a pending nested run can block a pending exception.
		 * Otherwise an injected NMI/interrupt should either be
		 * lost or delivered to the nested hypervisor in the EXITINTINFO
		 * vmcb field, while delivering the pending exception.
		 */
		if (svm->nested.nested_run_pending)
                        return -EBUSY;
		if (!nested_exit_on_exception(svm))
			return 0;
		nested_svm_inject_exception_vmexit(svm);
		return 0;
	}

	if (vcpu->arch.smi_pending && !svm_smi_blocked(vcpu)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_smi(svm))
			return 0;
		nested_svm_simple_vmexit(svm, SVM_EXIT_SMI);
		return 0;
	}

	if (vcpu->arch.nmi_pending && !svm_nmi_blocked(vcpu)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_nmi(svm))
			return 0;
		nested_svm_simple_vmexit(svm, SVM_EXIT_NMI);
		return 0;
	}

	if (kvm_cpu_has_interrupt(vcpu) && !svm_interrupt_blocked(vcpu)) {
		if (block_nested_events)
			return -EBUSY;
		if (!nested_exit_on_intr(svm))
			return 0;
		trace_kvm_nested_intr_vmexit(svm->vmcb->save.rip);
		nested_svm_simple_vmexit(svm, SVM_EXIT_INTR);
		return 0;
	}

	return 0;
}