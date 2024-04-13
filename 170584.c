static struct atalk_addr *__aarp_proxy_find(struct net_device *dev,
					    struct atalk_addr *sa)
{
	int hash = sa->s_node % (AARP_HASH_SIZE - 1);
	struct aarp_entry *a = __aarp_find_entry(proxies[hash], dev, sa);

	return a ? sa : NULL;
}