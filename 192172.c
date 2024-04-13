static void mce_disable_error_reporting(void)
{
	int i;

	for (i = 0; i < mca_cfg.banks; i++) {
		struct mce_bank *b = &mce_banks[i];

		if (b->init)
			wrmsrl(msr_ops.ctl(i), 0);
	}
	return;
}