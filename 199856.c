static void addrconf_add_linklocal(struct inet6_dev *idev, const struct in6_addr *addr)
{
	struct inet6_ifaddr *ifp;
	u32 addr_flags = IFA_F_PERMANENT;

#ifdef CONFIG_IPV6_OPTIMISTIC_DAD
	if (idev->cnf.optimistic_dad &&
	    !dev_net(idev->dev)->ipv6.devconf_all->forwarding)
		addr_flags |= IFA_F_OPTIMISTIC;
#endif


	ifp = ipv6_add_addr(idev, addr, NULL, 64, IFA_LINK, addr_flags, 0, 0);
	if (!IS_ERR(ifp)) {
		addrconf_prefix_route(&ifp->addr, ifp->prefix_len, idev->dev, 0, 0);
		addrconf_dad_start(ifp);
		in6_ifa_put(ifp);
	}
}