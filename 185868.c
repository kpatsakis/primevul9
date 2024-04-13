int ipv6_route_ioctl(struct net *net, unsigned int cmd, void __user *arg)
{
	struct fib6_config cfg;
	struct in6_rtmsg rtmsg;
	int err;

	switch (cmd) {
	case SIOCADDRT:		/* Add a route */
	case SIOCDELRT:		/* Delete a route */
		if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
			return -EPERM;
		err = copy_from_user(&rtmsg, arg,
				     sizeof(struct in6_rtmsg));
		if (err)
			return -EFAULT;

		rtmsg_to_fib6_config(net, &rtmsg, &cfg);

		rtnl_lock();
		switch (cmd) {
		case SIOCADDRT:
			err = ip6_route_add(&cfg);
			break;
		case SIOCDELRT:
			err = ip6_route_del(&cfg);
			break;
		default:
			err = -EINVAL;
		}
		rtnl_unlock();

		return err;
	}

	return -EINVAL;
}