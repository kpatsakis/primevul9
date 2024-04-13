static void __mcheck_cpu_init_clear_banks(void)
{
	int i;

	for (i = 0; i < mca_cfg.banks; i++) {
		struct mce_bank *b = &mce_banks[i];

		if (!b->init)
			continue;
		wrmsrl(msr_ops.ctl(i), b->ctl);
		wrmsrl(msr_ops.status(i), 0);
	}
}