static int copyin(void *to, const void __user *from, size_t n)
{
	if (should_fail_usercopy())
		return n;
	if (access_ok(from, n)) {
		instrument_copy_from_user(to, from, n);
		n = raw_copy_from_user(to, from, n);
	}
	return n;
}