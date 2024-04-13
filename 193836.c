u64 cpu_clock(int cpu)
{
	if (!sched_clock_stable())
		return sched_clock_cpu(cpu);

	return sched_clock();
}