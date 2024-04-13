static inline struct sched_clock_data *this_scd(void)
{
	return &__get_cpu_var(sched_clock_data);
}