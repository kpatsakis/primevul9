static void _warn_unseeded_randomness(const char *func_name, void *caller,
				      void **previous)
{
#ifdef CONFIG_WARN_ALL_UNSEEDED_RANDOM
	const bool print_once = false;
#else
	static bool print_once __read_mostly;
#endif

	if (print_once ||
	    crng_ready() ||
	    (previous && (caller == READ_ONCE(*previous))))
		return;
	WRITE_ONCE(*previous, caller);
#ifndef CONFIG_WARN_ALL_UNSEEDED_RANDOM
	print_once = true;
#endif
	if (__ratelimit(&unseeded_warning))
		printk_deferred(KERN_NOTICE "random: %s called from %pS "
				"with crng_init=%d\n", func_name, caller,
				crng_init);
}