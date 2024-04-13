static bool guest_can_use_gbpages(struct kvm_vcpu *vcpu)
{
	/*
	 * If TDP is enabled, let the guest use GBPAGES if they're supported in
	 * hardware.  The hardware page walker doesn't let KVM disable GBPAGES,
	 * i.e. won't treat them as reserved, and KVM doesn't redo the GVA->GPA
	 * walk for performance and complexity reasons.  Not to mention KVM
	 * _can't_ solve the problem because GVA->GPA walks aren't visible to
	 * KVM once a TDP translation is installed.  Mimic hardware behavior so
	 * that KVM's is at least consistent, i.e. doesn't randomly inject #PF.
	 */
	return tdp_enabled ? boot_cpu_has(X86_FEATURE_GBPAGES) :
			     guest_cpuid_has(vcpu, X86_FEATURE_GBPAGES);
}