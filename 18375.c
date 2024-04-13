static bool __init crng_init_try_arch_early(struct crng_state *crng)
{
	int		i;
	bool		arch_init = true;
	unsigned long	rv;

	for (i = 4; i < 16; i++) {
		if (!arch_get_random_seed_long_early(&rv) &&
		    !arch_get_random_long_early(&rv)) {
			rv = random_get_entropy();
			arch_init = false;
		}
		crng->state[i] ^= rv;
	}

	return arch_init;
}