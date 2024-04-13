static void __ipv6_ifa_notify(int event, struct inet6_ifaddr *ifp)
{
	struct net *net = dev_net(ifp->idev->dev);

	inet6_ifa_notify(event ? : RTM_NEWADDR, ifp);

	switch (event) {
	case RTM_NEWADDR:
		update_valid_ll_addr_cnt(ifp, 1);

		/*
		 * If the address was optimistic
		 * we inserted the route at the start of
		 * our DAD process, so we don't need
		 * to do it again
		 */
		if (!(ifp->rt->rt6i_node))
			ip6_ins_rt(ifp->rt);
		if (ifp->idev->cnf.forwarding)
			addrconf_join_anycast(ifp);
		if (!ipv6_addr_any(&ifp->peer_addr))
			addrconf_prefix_route(&ifp->peer_addr, 128,
					      ifp->idev->dev, 0, 0);
		break;
	case RTM_DELADDR:
		update_valid_ll_addr_cnt(ifp, -1);

		if (ifp->idev->cnf.forwarding)
			addrconf_leave_anycast(ifp);
		addrconf_leave_solict(ifp->idev, &ifp->addr);
		if (!ipv6_addr_any(&ifp->peer_addr)) {
			struct rt6_info *rt;
			struct net_device *dev = ifp->idev->dev;

			rt = rt6_lookup(dev_net(dev), &ifp->peer_addr, NULL,
					dev->ifindex, 1);
			if (rt) {
				dst_hold(&rt->dst);
				if (ip6_del_rt(rt))
					dst_free(&rt->dst);
			}
		}
		dst_hold(&ifp->rt->dst);

		if (ip6_del_rt(ifp->rt))
			dst_free(&ifp->rt->dst);
		break;
	}
	atomic_inc(&net->ipv6.dev_addr_genid);
}