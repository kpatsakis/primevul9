static int addrconf_disable_ipv6(struct ctl_table *table, int *p, int newf)
{
	struct net *net;
	int old;

	if (!rtnl_trylock())
		return restart_syscall();

	net = (struct net *)table->extra2;
	old = *p;
	*p = newf;

	if (p == &net->ipv6.devconf_dflt->disable_ipv6) {
		rtnl_unlock();
		return 0;
	}

	if (p == &net->ipv6.devconf_all->disable_ipv6) {
		net->ipv6.devconf_dflt->disable_ipv6 = newf;
		addrconf_disable_change(net, newf);
	} else if ((!newf) ^ (!old))
		dev_disable_change((struct inet6_dev *)table->extra1);

	rtnl_unlock();
	return 0;
}