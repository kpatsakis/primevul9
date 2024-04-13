static void igmp_mod_timer(struct ip_mc_list *im, int max_delay)
{
	spin_lock_bh(&im->lock);
	im->unsolicit_count = 0;
	if (del_timer(&im->timer)) {
		if ((long)(im->timer.expires-jiffies) < max_delay) {
			add_timer(&im->timer);
			im->tm_running = 1;
			spin_unlock_bh(&im->lock);
			return;
		}
		refcount_dec(&im->refcnt);
	}
	igmp_start_timer(im, max_delay);
	spin_unlock_bh(&im->lock);
}