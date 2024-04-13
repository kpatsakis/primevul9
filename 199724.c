static void sit_add_v4_addrs(struct inet6_dev *idev)
{
	struct in6_addr addr;
	struct net_device *dev;
	struct net *net = dev_net(idev->dev);
	int scope;

	ASSERT_RTNL();

	memset(&addr, 0, sizeof(struct in6_addr));
	memcpy(&addr.s6_addr32[3], idev->dev->dev_addr, 4);

	if (idev->dev->flags&IFF_POINTOPOINT) {
		addr.s6_addr32[0] = htonl(0xfe800000);
		scope = IFA_LINK;
	} else {
		scope = IPV6_ADDR_COMPATv4;
	}

	if (addr.s6_addr32[3]) {
		add_addr(idev, &addr, 128, scope);
		return;
	}

	for_each_netdev(net, dev) {
		struct in_device *in_dev = __in_dev_get_rtnl(dev);
		if (in_dev && (dev->flags & IFF_UP)) {
			struct in_ifaddr *ifa;

			int flag = scope;

			for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
				int plen;

				addr.s6_addr32[3] = ifa->ifa_local;

				if (ifa->ifa_scope == RT_SCOPE_LINK)
					continue;
				if (ifa->ifa_scope >= RT_SCOPE_HOST) {
					if (idev->dev->flags&IFF_POINTOPOINT)
						continue;
					flag |= IFA_HOST;
				}
				if (idev->dev->flags&IFF_POINTOPOINT)
					plen = 64;
				else
					plen = 96;

				add_addr(idev, &addr, plen, flag);
			}
		}
	}
}