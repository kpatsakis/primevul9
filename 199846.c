int addrconf_set_dstaddr(struct net *net, void __user *arg)
{
	struct in6_ifreq ireq;
	struct net_device *dev;
	int err = -EINVAL;

	rtnl_lock();

	err = -EFAULT;
	if (copy_from_user(&ireq, arg, sizeof(struct in6_ifreq)))
		goto err_exit;

	dev = __dev_get_by_index(net, ireq.ifr6_ifindex);

	err = -ENODEV;
	if (dev == NULL)
		goto err_exit;

#if IS_ENABLED(CONFIG_IPV6_SIT)
	if (dev->type == ARPHRD_SIT) {
		const struct net_device_ops *ops = dev->netdev_ops;
		struct ifreq ifr;
		struct ip_tunnel_parm p;

		err = -EADDRNOTAVAIL;
		if (!(ipv6_addr_type(&ireq.ifr6_addr) & IPV6_ADDR_COMPATv4))
			goto err_exit;

		memset(&p, 0, sizeof(p));
		p.iph.daddr = ireq.ifr6_addr.s6_addr32[3];
		p.iph.saddr = 0;
		p.iph.version = 4;
		p.iph.ihl = 5;
		p.iph.protocol = IPPROTO_IPV6;
		p.iph.ttl = 64;
		ifr.ifr_ifru.ifru_data = (__force void __user *)&p;

		if (ops->ndo_do_ioctl) {
			mm_segment_t oldfs = get_fs();

			set_fs(KERNEL_DS);
			err = ops->ndo_do_ioctl(dev, &ifr, SIOCADDTUNNEL);
			set_fs(oldfs);
		} else
			err = -EOPNOTSUPP;

		if (err == 0) {
			err = -ENOBUFS;
			dev = __dev_get_by_name(net, p.name);
			if (!dev)
				goto err_exit;
			err = dev_open(dev);
		}
	}
#endif

err_exit:
	rtnl_unlock();
	return err;
}