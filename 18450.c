static void __init crng_initialize_primary(struct crng_state *crng)
{
	memcpy(&crng->state[0], "expand 32-byte k", 16);
	_extract_entropy(&input_pool, &crng->state[4], sizeof(__u32) * 12, 0);
	if (crng_init_try_arch_early(crng) && trust_cpu) {
		invalidate_batched_entropy();
		numa_crng_init();
		crng_init = 2;
		pr_notice("crng done (trusting CPU's manufacturer)\n");
	}
	crng->init_time = jiffies - CRNG_RESEED_INTERVAL - 1;
}