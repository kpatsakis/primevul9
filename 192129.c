void mcheck_cpu_init(struct cpuinfo_x86 *c)
{
	if (mca_cfg.disabled)
		return;

	if (__mcheck_cpu_ancient_init(c))
		return;

	if (!mce_available(c))
		return;

	if (__mcheck_cpu_cap_init() < 0 || __mcheck_cpu_apply_quirks(c) < 0) {
		mca_cfg.disabled = true;
		return;
	}

	if (mce_gen_pool_init()) {
		mca_cfg.disabled = true;
		pr_emerg("Couldn't allocate MCE records pool!\n");
		return;
	}

	machine_check_vector = do_machine_check;

	__mcheck_cpu_init_early(c);
	__mcheck_cpu_init_generic();
	__mcheck_cpu_init_vendor(c);
	__mcheck_cpu_init_clear_banks();
	__mcheck_cpu_setup_timer();
}