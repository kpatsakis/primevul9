static void _extract_crng(struct crng_state *crng,
			  __u8 out[CHACHA_BLOCK_SIZE])
{
	unsigned long v, flags;

	if (crng_ready() &&
	    (time_after(crng_global_init_time, crng->init_time) ||
	     time_after(jiffies, crng->init_time + CRNG_RESEED_INTERVAL)))
		crng_reseed(crng, crng == &primary_crng ? &input_pool : NULL);
	spin_lock_irqsave(&crng->lock, flags);
	if (arch_get_random_long(&v))
		crng->state[14] ^= v;
	chacha20_block(&crng->state[0], out);
	if (crng->state[12] == 0)
		crng->state[13]++;
	spin_unlock_irqrestore(&crng->lock, flags);
}