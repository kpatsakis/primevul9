static void mce_reenable_cpu(void)
{
	int i;

	if (!mce_available(raw_cpu_ptr(&cpu_info)))
		return;

	if (!cpuhp_tasks_frozen)
		cmci_reenable();
	for (i = 0; i < mca_cfg.banks; i++) {
		struct mce_bank *b = &mce_banks[i];

		if (b->init)
			wrmsrl(msr_ops.ctl(i), b->ctl);
	}
}