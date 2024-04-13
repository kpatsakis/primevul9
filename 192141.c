static void mce_reset(void)
{
	cpu_missing = 0;
	atomic_set(&mce_fake_panicked, 0);
	atomic_set(&mce_executing, 0);
	atomic_set(&mce_callin, 0);
	atomic_set(&global_nwo, 0);
}