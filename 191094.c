static __init void svm_set_cpu_caps(void)
{
	kvm_set_cpu_caps();

	supported_xss = 0;

	/* CPUID 0x80000001 and 0x8000000A (SVM features) */
	if (nested) {
		kvm_cpu_cap_set(X86_FEATURE_SVM);

		if (nrips)
			kvm_cpu_cap_set(X86_FEATURE_NRIPS);

		if (npt_enabled)
			kvm_cpu_cap_set(X86_FEATURE_NPT);
	}

	/* CPUID 0x80000008 */
	if (boot_cpu_has(X86_FEATURE_LS_CFG_SSBD) ||
	    boot_cpu_has(X86_FEATURE_AMD_SSBD))
		kvm_cpu_cap_set(X86_FEATURE_VIRT_SSBD);
}