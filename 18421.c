static void crng_reseed(struct crng_state *crng, struct entropy_store *r)
{
	unsigned long	flags;
	int		i, num;
	union {
		__u8	block[CHACHA_BLOCK_SIZE];
		__u32	key[8];
	} buf;

	if (r) {
		num = extract_entropy(r, &buf, 32, 16, 0);
		if (num == 0)
			return;
	} else {
		_extract_crng(&primary_crng, buf.block);
		_crng_backtrack_protect(&primary_crng, buf.block,
					CHACHA_KEY_SIZE);
	}
	spin_lock_irqsave(&crng->lock, flags);
	for (i = 0; i < 8; i++) {
		unsigned long	rv;
		if (!arch_get_random_seed_long(&rv) &&
		    !arch_get_random_long(&rv))
			rv = random_get_entropy();
		crng->state[i+4] ^= buf.key[i] ^ rv;
	}
	memzero_explicit(&buf, sizeof(buf));
	crng->init_time = jiffies;
	spin_unlock_irqrestore(&crng->lock, flags);
	if (crng == &primary_crng && crng_init < 2) {
		invalidate_batched_entropy();
		numa_crng_init();
		crng_init = 2;
		process_random_ready_list();
		wake_up_interruptible(&crng_init_wait);
		kill_fasync(&fasync, SIGIO, POLL_IN);
		pr_notice("crng init done\n");
		if (unseeded_warning.missed) {
			pr_notice("%d get_random_xx warning(s) missed due to ratelimiting\n",
				  unseeded_warning.missed);
			unseeded_warning.missed = 0;
		}
		if (urandom_warning.missed) {
			pr_notice("%d urandom warning(s) missed due to ratelimiting\n",
				  urandom_warning.missed);
			urandom_warning.missed = 0;
		}
	}
}