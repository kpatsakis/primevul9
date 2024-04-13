static void __mcheck_cpu_init_generic(void)
{
	enum mcp_flags m_fl = 0;
	mce_banks_t all_banks;
	u64 cap;

	if (!mca_cfg.bootlog)
		m_fl = MCP_DONTLOG;

	/*
	 * Log the machine checks left over from the previous reset.
	 */
	bitmap_fill(all_banks, MAX_NR_BANKS);
	machine_check_poll(MCP_UC | m_fl, &all_banks);

	cr4_set_bits(X86_CR4_MCE);

	rdmsrl(MSR_IA32_MCG_CAP, cap);
	if (cap & MCG_CTL_P)
		wrmsr(MSR_IA32_MCG_CTL, 0xffffffff, 0xffffffff);
}