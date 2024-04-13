static inline long do_strnlen_user(const char __user *src, unsigned long count, unsigned long max)
{
	const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;
	long align, res = 0;
	unsigned long c;

	/*
	 * Truncate 'max' to the user-specified limit, so that
	 * we only have one limit we need to check in the loop
	 */
	if (max > count)
		max = count;

	/*
	 * Do everything aligned. But that means that we
	 * need to also expand the maximum..
	 */
	align = (sizeof(long) - 1) & (unsigned long)src;
	src -= align;
	max += align;

	unsafe_get_user(c, (unsigned long __user *)src, efault);
	c |= aligned_byte_mask(align);

	for (;;) {
		unsigned long data;
		if (has_zero(c, &data, &constants)) {
			data = prep_zero_mask(c, data, &constants);
			data = create_zero_mask(data);
			return res + find_zero(data) + 1 - align;
		}
		res += sizeof(unsigned long);
		/* We already handled 'unsigned long' bytes. Did we do it all ? */
		if (unlikely(max <= sizeof(unsigned long)))
			break;
		max -= sizeof(unsigned long);
		unsafe_get_user(c, (unsigned long __user *)(src+res), efault);
	}
	res -= align;

	/*
	 * Uhhuh. We hit 'max'. But was that the user-specified maximum
	 * too? If so, return the marker for "too long".
	 */
	if (res >= count)
		return count+1;

	/*
	 * Nope: we hit the address space limit, and we still had more
	 * characters the caller would have wanted. That's 0.
	 */
efault:
	return 0;
}