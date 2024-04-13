static long get_nx_lpage_recovery_timeout(u64 start_time)
{
	bool enabled;
	uint period;

	enabled = calc_nx_huge_pages_recovery_period(&period);

	return enabled ? start_time + msecs_to_jiffies(period) - get_jiffies_64()
		       : MAX_SCHEDULE_TIMEOUT;
}