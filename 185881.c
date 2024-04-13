int ip6_route_get_saddr(struct net *net,
			struct rt6_info *rt,
			const struct in6_addr *daddr,
			unsigned int prefs,
			struct in6_addr *saddr)
{
	struct inet6_dev *idev = ip6_dst_idev((struct dst_entry *)rt);
	int err = 0;
	if (rt->rt6i_prefsrc.plen)
		*saddr = rt->rt6i_prefsrc.addr;
	else
		err = ipv6_dev_get_saddr(net, idev ? idev->dev : NULL,
					 daddr, prefs, saddr);
	return err;
}