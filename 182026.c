unsigned int kvm_arch_para_features(void)
{
	return cpuid_eax(kvm_cpuid_base() | KVM_CPUID_FEATURES);
}