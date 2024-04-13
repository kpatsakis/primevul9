static int copyout_mc(void __user *to, const void *from, size_t n)
{
	if (access_ok(to, n)) {
		instrument_copy_to_user(to, from, n);
		n = copy_mc_to_user((__force void *) to, from, n);
	}
	return n;
}