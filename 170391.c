int bpf_check_uarg_tail_zero(void __user *uaddr,
			     size_t expected_size,
			     size_t actual_size)
{
	unsigned char __user *addr = uaddr + expected_size;
	int res;

	if (unlikely(actual_size > PAGE_SIZE))	/* silly large */
		return -E2BIG;

	if (actual_size <= expected_size)
		return 0;

	res = check_zeroed_user(addr, actual_size - expected_size);
	if (res < 0)
		return res;
	return res ? 0 : -E2BIG;
}