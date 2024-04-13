static void _crng_backtrack_protect(struct crng_state *crng,
				    __u8 tmp[CHACHA_BLOCK_SIZE], int used)
{
	unsigned long	flags;
	__u32		*s, *d;
	int		i;

	used = round_up(used, sizeof(__u32));
	if (used + CHACHA_KEY_SIZE > CHACHA_BLOCK_SIZE) {
		extract_crng(tmp);
		used = 0;
	}
	spin_lock_irqsave(&crng->lock, flags);
	s = (__u32 *) &tmp[used];
	d = &crng->state[4];
	for (i=0; i < 8; i++)
		*d++ ^= *s++;
	spin_unlock_irqrestore(&crng->lock, flags);
}