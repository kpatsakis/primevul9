static void svm_cpuid_update(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	vcpu->arch.xsaves_enabled = guest_cpuid_has(vcpu, X86_FEATURE_XSAVE) &&
				    boot_cpu_has(X86_FEATURE_XSAVE) &&
				    boot_cpu_has(X86_FEATURE_XSAVES);

	/* Update nrips enabled cache */
	svm->nrips_enabled = kvm_cpu_cap_has(X86_FEATURE_NRIPS) &&
			     guest_cpuid_has(&svm->vcpu, X86_FEATURE_NRIPS);

	if (!kvm_vcpu_apicv_active(vcpu))
		return;

	/*
	 * AVIC does not work with an x2APIC mode guest. If the X2APIC feature
	 * is exposed to the guest, disable AVIC.
	 */
	if (guest_cpuid_has(vcpu, X86_FEATURE_X2APIC))
		kvm_request_apicv_update(vcpu->kvm, false,
					 APICV_INHIBIT_REASON_X2APIC);

	/*
	 * Currently, AVIC does not work with nested virtualization.
	 * So, we disable AVIC when cpuid for SVM is set in the L1 guest.
	 */
	if (nested && guest_cpuid_has(vcpu, X86_FEATURE_SVM))
		kvm_request_apicv_update(vcpu->kvm, false,
					 APICV_INHIBIT_REASON_NESTED);
}