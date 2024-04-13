bool kvm_para_available(void)
{
	return kvm_cpuid_base() != 0;
}