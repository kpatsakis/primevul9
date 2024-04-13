static bool crng_init_try_arch(struct crng_state *crng)
{
	int		i;
	bool		arch_init = true;
	unsigned long	rv;

	for (i = 4; i < 16; i++) {
		if (!arch_get_random_seed_long(&rv) &&
		    !arch_get_random_long(&rv)) {
			rv = random_get_entropy();
			arch_init = false;
		}
		crng->state[i] ^= rv;
	}

	return arch_init;
}