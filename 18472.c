void add_hwgenerator_randomness(const char *buffer, size_t count,
				size_t entropy)
{
	struct entropy_store *poolp = &input_pool;

	if (unlikely(crng_init == 0)) {
		crng_fast_load(buffer, count);
		return;
	}

	/* Suspend writing if we're above the trickle threshold.
	 * We'll be woken up again once below random_write_wakeup_thresh,
	 * or when the calling thread is about to terminate.
	 */
	wait_event_interruptible(random_write_wait, kthread_should_stop() ||
			ENTROPY_BITS(&input_pool) <= random_write_wakeup_bits);
	mix_pool_bytes(poolp, buffer, count);
	credit_entropy_bits(poolp, entropy);
}