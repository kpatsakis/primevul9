static int __mcheck_cpu_cap_init(void)
{
	unsigned b;
	u64 cap;

	rdmsrl(MSR_IA32_MCG_CAP, cap);

	b = cap & MCG_BANKCNT_MASK;
	if (!mca_cfg.banks)
		pr_info("CPU supports %d MCE banks\n", b);

	if (b > MAX_NR_BANKS) {
		pr_warn("Using only %u machine check banks out of %u\n",
			MAX_NR_BANKS, b);
		b = MAX_NR_BANKS;
	}

	/* Don't support asymmetric configurations today */
	WARN_ON(mca_cfg.banks != 0 && b != mca_cfg.banks);
	mca_cfg.banks = b;

	if (!mce_banks) {
		int err = __mcheck_cpu_mce_banks_init();

		if (err)
			return err;
	}

	/* Use accurate RIP reporting if available. */
	if ((cap & MCG_EXT_P) && MCG_EXT_CNT(cap) >= 9)
		mca_cfg.rip_msr = MSR_IA32_MCG_EIP;

	if (cap & MCG_SER_P)
		mca_cfg.ser = true;

	return 0;
}