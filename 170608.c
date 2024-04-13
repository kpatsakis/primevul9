static void __aarp_expire_device(struct aarp_entry **n, struct net_device *dev)
{
	struct aarp_entry *t;

	while (*n)
		if ((*n)->dev == dev) {
			t = *n;
			*n = (*n)->next;
			__aarp_expire(t);
		} else
			n = &((*n)->next);
}