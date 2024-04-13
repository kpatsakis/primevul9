static void igmp_gq_timer_expire(struct timer_list *t)
{
	struct in_device *in_dev = from_timer(in_dev, t, mr_gq_timer);

	in_dev->mr_gq_running = 0;
	igmpv3_send_report(in_dev, NULL);
	in_dev_put(in_dev);
}