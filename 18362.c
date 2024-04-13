int wait_for_random_bytes(void)
{
	if (likely(crng_ready()))
		return 0;

	do {
		int ret;
		ret = wait_event_interruptible_timeout(crng_init_wait, crng_ready(), HZ);
		if (ret)
			return ret > 0 ? 0 : ret;

		try_to_generate_entropy();
	} while (!crng_ready());

	return 0;
}