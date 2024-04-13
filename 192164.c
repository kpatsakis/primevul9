static void mce_clear_state(unsigned long *toclear)
{
	int i;

	for (i = 0; i < mca_cfg.banks; i++) {
		if (test_bit(i, toclear))
			mce_wrmsrl(msr_ops.status(i), 0);
	}
}