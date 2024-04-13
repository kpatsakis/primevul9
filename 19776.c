void sev_es_init_vmcb(struct vcpu_svm *svm)
{
	struct kvm_vcpu *vcpu = &svm->vcpu;

	svm->vmcb->control.nested_ctl |= SVM_NESTED_CTL_SEV_ES_ENABLE;
	svm->vmcb->control.virt_ext |= LBR_CTL_ENABLE_MASK;

	/*
	 * An SEV-ES guest requires a VMSA area that is a separate from the
	 * VMCB page. Do not include the encryption mask on the VMSA physical
	 * address since hardware will access it using the guest key.
	 */
	svm->vmcb->control.vmsa_pa = __pa(svm->vmsa);

	/* Can't intercept CR register access, HV can't modify CR registers */
	svm_clr_intercept(svm, INTERCEPT_CR0_READ);
	svm_clr_intercept(svm, INTERCEPT_CR4_READ);
	svm_clr_intercept(svm, INTERCEPT_CR8_READ);
	svm_clr_intercept(svm, INTERCEPT_CR0_WRITE);
	svm_clr_intercept(svm, INTERCEPT_CR4_WRITE);
	svm_clr_intercept(svm, INTERCEPT_CR8_WRITE);

	svm_clr_intercept(svm, INTERCEPT_SELECTIVE_CR0);

	/* Track EFER/CR register changes */
	svm_set_intercept(svm, TRAP_EFER_WRITE);
	svm_set_intercept(svm, TRAP_CR0_WRITE);
	svm_set_intercept(svm, TRAP_CR4_WRITE);
	svm_set_intercept(svm, TRAP_CR8_WRITE);

	/* No support for enable_vmware_backdoor */
	clr_exception_intercept(svm, GP_VECTOR);

	/* Can't intercept XSETBV, HV can't modify XCR0 directly */
	svm_clr_intercept(svm, INTERCEPT_XSETBV);

	/* Clear intercepts on selected MSRs */
	set_msr_interception(vcpu, svm->msrpm, MSR_EFER, 1, 1);
	set_msr_interception(vcpu, svm->msrpm, MSR_IA32_CR_PAT, 1, 1);
	set_msr_interception(vcpu, svm->msrpm, MSR_IA32_LASTBRANCHFROMIP, 1, 1);
	set_msr_interception(vcpu, svm->msrpm, MSR_IA32_LASTBRANCHTOIP, 1, 1);
	set_msr_interception(vcpu, svm->msrpm, MSR_IA32_LASTINTFROMIP, 1, 1);
	set_msr_interception(vcpu, svm->msrpm, MSR_IA32_LASTINTTOIP, 1, 1);
}