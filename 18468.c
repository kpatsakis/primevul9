static inline struct timer_base *get_timer_this_cpu_base(u32 tflags)
{
	struct timer_base *base = this_cpu_ptr(&timer_bases[BASE_STD]);

	/*
	 * If the timer is deferrable and NO_HZ_COMMON is set then we need
	 * to use the deferrable base.
	 */
	if (IS_ENABLED(CONFIG_NO_HZ_COMMON) && (tflags & TIMER_DEFERRABLE))
		base = this_cpu_ptr(&timer_bases[BASE_DEF]);
	return base;
}