static int crng_fast_load(const char *cp, size_t len)
{
	unsigned long flags;
	char *p;

	if (!spin_trylock_irqsave(&primary_crng.lock, flags))
		return 0;
	if (crng_init != 0) {
		spin_unlock_irqrestore(&primary_crng.lock, flags);
		return 0;
	}
	p = (unsigned char *) &primary_crng.state[4];
	while (len > 0 && crng_init_cnt < CRNG_INIT_CNT_THRESH) {
		p[crng_init_cnt % CHACHA_KEY_SIZE] ^= *cp;
		cp++; crng_init_cnt++; len--;
	}
	spin_unlock_irqrestore(&primary_crng.lock, flags);
	if (crng_init_cnt >= CRNG_INIT_CNT_THRESH) {
		invalidate_batched_entropy();
		crng_init = 1;
		pr_notice("fast init done\n");
	}
	return 1;
}