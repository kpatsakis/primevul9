static void aarp_expire_timeout(struct timer_list *unused)
{
	int ct;

	write_lock_bh(&aarp_lock);

	for (ct = 0; ct < AARP_HASH_SIZE; ct++) {
		__aarp_expire_timer(&resolved[ct]);
		__aarp_kick(&unresolved[ct]);
		__aarp_expire_timer(&unresolved[ct]);
		__aarp_expire_timer(&proxies[ct]);
	}

	write_unlock_bh(&aarp_lock);
	mod_timer(&aarp_timer, jiffies +
			       (unresolved_count ? sysctl_aarp_tick_time :
				sysctl_aarp_expiry_time));
}