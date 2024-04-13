static void __maybe_unused crng_initialize_secondary(struct crng_state *crng)
{
	memcpy(&crng->state[0], "expand 32-byte k", 16);
	_get_random_bytes(&crng->state[4], sizeof(__u32) * 12);
	crng_init_try_arch(crng);
	crng->init_time = jiffies - CRNG_RESEED_INTERVAL - 1;
}