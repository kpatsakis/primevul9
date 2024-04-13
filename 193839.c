void sched_clock_idle_sleep_event(void)
{
	sched_clock_cpu(smp_processor_id());
}