static int addrconf_fixup_forwarding(struct ctl_table *table, int *p, int newf)
{
	struct net *net;
	int old;

	if (!rtnl_trylock())
		return restart_syscall();

	net = (struct net *)table->extra2;
	old = *p;
	*p = newf;

	if (p == &net->ipv6.devconf_dflt->forwarding) {
		if ((!newf) ^ (!old))
			inet6_netconf_notify_devconf(net, NETCONFA_FORWARDING,
						     NETCONFA_IFINDEX_DEFAULT,
						     net->ipv6.devconf_dflt);
		rtnl_unlock();
		return 0;
	}

	if (p == &net->ipv6.devconf_all->forwarding) {
		net->ipv6.devconf_dflt->forwarding = newf;
		addrconf_forward_change(net, newf);
		if ((!newf) ^ (!old))
			inet6_netconf_notify_devconf(net, NETCONFA_FORWARDING,
						     NETCONFA_IFINDEX_ALL,
						     net->ipv6.devconf_all);
	} else if ((!newf) ^ (!old))
		dev_forward_change((struct inet6_dev *)table->extra1);
	rtnl_unlock();

	if (newf)
		rt6_purge_dflt_routers(net);
	return 1;
}