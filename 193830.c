unsigned long long __attribute__((weak)) sched_clock(void)
{
	return (unsigned long long)(jiffies - INITIAL_JIFFIES)
					* (NSEC_PER_SEC / HZ);
}