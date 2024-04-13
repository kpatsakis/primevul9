static inline void load_kernel_asce(void)
{
	unsigned long asce;

	__ctl_store(asce, 1, 1);
	if (asce != S390_lowcore.kernel_asce)
		__ctl_load(S390_lowcore.kernel_asce, 1, 1);
	set_cpu_flag(CIF_ASCE);
}