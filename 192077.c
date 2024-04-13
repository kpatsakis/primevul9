int mce_available(struct cpuinfo_x86 *c)
{
	if (mca_cfg.disabled)
		return 0;
	return cpu_has(c, X86_FEATURE_MCE) && cpu_has(c, X86_FEATURE_MCA);
}