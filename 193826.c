static inline struct sched_clock_data *cpu_sdc(int cpu)
{
	return &per_cpu(sched_clock_data, cpu);
}