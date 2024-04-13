static inline uint32_t kvm_cpuid_base(void)
{
	static int kvm_cpuid_base = -1;

	if (kvm_cpuid_base == -1)
		kvm_cpuid_base = __kvm_cpuid_base();

	return kvm_cpuid_base;
}