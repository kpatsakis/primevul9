int addrconf_del_ifaddr(struct net *net, void __user *arg)
{
	struct in6_ifreq ireq;
	int err;

	if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	if (copy_from_user(&ireq, arg, sizeof(struct in6_ifreq)))
		return -EFAULT;

	rtnl_lock();
	err = inet6_addr_del(net, ireq.ifr6_ifindex, &ireq.ifr6_addr,
			     ireq.ifr6_prefixlen);
	rtnl_unlock();
	return err;
}