static __be32 igmpv3_get_srcaddr(struct net_device *dev,
				 const struct flowi4 *fl4)
{
	struct in_device *in_dev = __in_dev_get_rcu(dev);
	const struct in_ifaddr *ifa;

	if (!in_dev)
		return htonl(INADDR_ANY);

	in_dev_for_each_ifa_rcu(ifa, in_dev) {
		if (fl4->saddr == ifa->ifa_local)
			return fl4->saddr;
	}

	return htonl(INADDR_ANY);
}