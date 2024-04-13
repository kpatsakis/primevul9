static uint32_t __init kvm_detect(void)
{
	return kvm_cpuid_base();
}