SYSCALL_DEFINE3(getrandom, char __user *, buf, size_t, count,
		unsigned int, flags)
{
	int ret;

	if (flags & ~(GRND_NONBLOCK|GRND_RANDOM|GRND_INSECURE))
		return -EINVAL;

	/*
	 * Requesting insecure and blocking randomness at the same time makes
	 * no sense.
	 */
	if ((flags & (GRND_INSECURE|GRND_RANDOM)) == (GRND_INSECURE|GRND_RANDOM))
		return -EINVAL;

	if (count > INT_MAX)
		count = INT_MAX;

	if (!(flags & GRND_INSECURE) && !crng_ready()) {
		if (flags & GRND_NONBLOCK)
			return -EAGAIN;
		ret = wait_for_random_bytes();
		if (unlikely(ret))
			return ret;
	}
	return urandom_read_nowarn(NULL, buf, count, NULL);
}