u64 local_clock(void)
{
	if (!sched_clock_stable())
		return sched_clock_cpu(raw_smp_processor_id());

	return sched_clock();
}