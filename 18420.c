int __init rand_initialize(void)
{
	init_std_data(&input_pool);
	crng_initialize_primary(&primary_crng);
	crng_global_init_time = jiffies;
	if (ratelimit_disable) {
		urandom_warning.interval = 0;
		unseeded_warning.interval = 0;
	}
	return 0;
}