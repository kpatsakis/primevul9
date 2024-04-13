static void igmp_stop_timer(struct ip_mc_list *im)
{
	spin_lock_bh(&im->lock);
	if (del_timer(&im->timer))
		refcount_dec(&im->refcnt);
	im->tm_running = 0;
	im->reporter = 0;
	im->unsolicit_count = 0;
	spin_unlock_bh(&im->lock);
}