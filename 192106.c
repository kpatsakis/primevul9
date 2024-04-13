static int fake_panic_get(void *data, u64 *val)
{
	*val = fake_panic;
	return 0;
}