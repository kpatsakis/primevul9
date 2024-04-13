void aarp_proxy_remove(struct net_device *dev, struct atalk_addr *sa)
{
	int hash = sa->s_node % (AARP_HASH_SIZE - 1);
	struct aarp_entry *a;

	write_lock_bh(&aarp_lock);

	a = __aarp_find_entry(proxies[hash], dev, sa);
	if (a)
		a->expires_at = jiffies - 1;

	write_unlock_bh(&aarp_lock);
}