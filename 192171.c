static int __mcheck_cpu_mce_banks_init(void)
{
	int i;
	u8 num_banks = mca_cfg.banks;

	mce_banks = kzalloc(num_banks * sizeof(struct mce_bank), GFP_KERNEL);
	if (!mce_banks)
		return -ENOMEM;

	for (i = 0; i < num_banks; i++) {
		struct mce_bank *b = &mce_banks[i];

		b->ctl = -1ULL;
		b->init = 1;
	}
	return 0;
}