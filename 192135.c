static void __mcheck_cpu_init_early(struct cpuinfo_x86 *c)
{
	if (c->x86_vendor == X86_VENDOR_AMD) {
		mce_flags.overflow_recov = !!cpu_has(c, X86_FEATURE_OVERFLOW_RECOV);
		mce_flags.succor	 = !!cpu_has(c, X86_FEATURE_SUCCOR);
		mce_flags.smca		 = !!cpu_has(c, X86_FEATURE_SMCA);

		if (mce_flags.smca) {
			msr_ops.ctl	= smca_ctl_reg;
			msr_ops.status	= smca_status_reg;
			msr_ops.addr	= smca_addr_reg;
			msr_ops.misc	= smca_misc_reg;
		}
	}
}