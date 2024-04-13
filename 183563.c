static struct in_device *ip_mc_find_dev(struct net *net, struct ip_mreqn *imr)
{
	struct net_device *dev = NULL;
	struct in_device *idev = NULL;

	if (imr->imr_ifindex) {
		idev = inetdev_by_index(net, imr->imr_ifindex);
		return idev;
	}
	if (imr->imr_address.s_addr) {
		dev = __ip_dev_find(net, imr->imr_address.s_addr, false);
		if (!dev)
			return NULL;
	}

	if (!dev) {
		struct rtable *rt = ip_route_output(net,
						    imr->imr_multiaddr.s_addr,
						    0, 0, 0);
		if (!IS_ERR(rt)) {
			dev = rt->dst.dev;
			ip_rt_put(rt);
		}
	}
	if (dev) {
		imr->imr_ifindex = dev->ifindex;
		idev = __in_dev_get_rtnl(dev);
	}
	return idev;
}