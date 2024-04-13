static void igmp_start_timer(struct ip_mc_list *im, int max_delay)
{
	int tv = prandom_u32() % max_delay;

	im->tm_running = 1;
	if (!mod_timer(&im->timer, jiffies+tv+2))
		refcount_inc(&im->refcnt);
}