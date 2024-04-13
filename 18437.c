static void add_timer_randomness(struct timer_rand_state *state, unsigned num)
{
	struct entropy_store	*r;
	struct {
		long jiffies;
		unsigned cycles;
		unsigned num;
	} sample;
	long delta, delta2, delta3;

	sample.jiffies = jiffies;
	sample.cycles = random_get_entropy();
	sample.num = num;
	r = &input_pool;
	mix_pool_bytes(r, &sample, sizeof(sample));

	/*
	 * Calculate number of bits of randomness we probably added.
	 * We take into account the first, second and third-order deltas
	 * in order to make our estimate.
	 */
	delta = sample.jiffies - READ_ONCE(state->last_time);
	WRITE_ONCE(state->last_time, sample.jiffies);

	delta2 = delta - READ_ONCE(state->last_delta);
	WRITE_ONCE(state->last_delta, delta);

	delta3 = delta2 - READ_ONCE(state->last_delta2);
	WRITE_ONCE(state->last_delta2, delta2);

	if (delta < 0)
		delta = -delta;
	if (delta2 < 0)
		delta2 = -delta2;
	if (delta3 < 0)
		delta3 = -delta3;
	if (delta > delta2)
		delta = delta2;
	if (delta > delta3)
		delta = delta3;

	/*
	 * delta is now minimum absolute delta.
	 * Round down by 1 bit on general principles,
	 * and limit entropy estimate to 12 bits.
	 */
	credit_entropy_bits(r, min_t(int, fls(delta>>1), 11));
}