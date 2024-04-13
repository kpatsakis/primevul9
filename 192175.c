void mcheck_cpu_clear(struct cpuinfo_x86 *c)
{
	if (mca_cfg.disabled)
		return;

	if (!mce_available(c))
		return;

	/*
	 * Possibly to clear general settings generic to x86
	 * __mcheck_cpu_clear_generic(c);
	 */
	__mcheck_cpu_clear_vendor(c);

}