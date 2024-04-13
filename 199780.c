static void dev_forward_change(struct inet6_dev *idev)
{
	struct net_device *dev;
	struct inet6_ifaddr *ifa;

	if (!idev)
		return;
	dev = idev->dev;
	if (idev->cnf.forwarding)
		dev_disable_lro(dev);
	if (dev->flags & IFF_MULTICAST) {
		if (idev->cnf.forwarding) {
			ipv6_dev_mc_inc(dev, &in6addr_linklocal_allrouters);
			ipv6_dev_mc_inc(dev, &in6addr_interfacelocal_allrouters);
			ipv6_dev_mc_inc(dev, &in6addr_sitelocal_allrouters);
		} else {
			ipv6_dev_mc_dec(dev, &in6addr_linklocal_allrouters);
			ipv6_dev_mc_dec(dev, &in6addr_interfacelocal_allrouters);
			ipv6_dev_mc_dec(dev, &in6addr_sitelocal_allrouters);
		}
	}

	list_for_each_entry(ifa, &idev->addr_list, if_list) {
		if (ifa->flags&IFA_F_TENTATIVE)
			continue;
		if (idev->cnf.forwarding)
			addrconf_join_anycast(ifa);
		else
			addrconf_leave_anycast(ifa);
	}
	inet6_netconf_notify_devconf(dev_net(dev), NETCONFA_FORWARDING,
				     dev->ifindex, &idev->cnf);
}