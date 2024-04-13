static inline long do_strncpy_from_user(char *dst, const char __user *src, long count, unsigned long max)
{
	const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;
	long res = 0;

	/*
	 * Truncate 'max' to the user-specified limit, so that
	 * we only have one limit we need to check in the loop
	 */
	if (max > count)
		max = count;

	if (IS_UNALIGNED(src, dst))
		goto byte_at_a_time;

	while (max >= sizeof(unsigned long)) {
		unsigned long c, data;

		/* Fall back to byte-at-a-time if we get a page fault */
		unsafe_get_user(c, (unsigned long __user *)(src+res), byte_at_a_time);

		*(unsigned long *)(dst+res) = c;
		if (has_zero(c, &data, &constants)) {
			data = prep_zero_mask(c, data, &constants);
			data = create_zero_mask(data);
			return res + find_zero(data);
		}
		res += sizeof(unsigned long);
		max -= sizeof(unsigned long);
	}

byte_at_a_time:
	while (max) {
		char c;

		unsafe_get_user(c,src+res, efault);
		dst[res] = c;
		if (!c)
			return res;
		res++;
		max--;
	}

	/*
	 * Uhhuh. We hit 'max'. But was that the user-specified maximum
	 * too? If so, that's ok - we got as much as the user asked for.
	 */
	if (res >= count)
		return res;

	/*
	 * Nope: we hit the address space limit, and we still had more
	 * characters the caller would have wanted. That's an EFAULT.
	 */
efault:
	return -EFAULT;
}