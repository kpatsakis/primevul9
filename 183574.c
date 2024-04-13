static void igmp_gq_start_timer(struct in_device *in_dev)
{
	int tv = prandom_u32() % in_dev->mr_maxdelay;
	unsigned long exp = jiffies + tv + 2;

	if (in_dev->mr_gq_running &&
	    time_after_eq(exp, (in_dev->mr_gq_timer).expires))
		return;

	in_dev->mr_gq_running = 1;
	if (!mod_timer(&in_dev->mr_gq_timer, exp))
		in_dev_hold(in_dev);
}