static int fake_panic_set(void *data, u64 val)
{
	mce_reset();
	fake_panic = val;
	return 0;
}