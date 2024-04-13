static void addrconf_leave_anycast(struct inet6_ifaddr *ifp)
{
	struct in6_addr addr;
	if (ifp->prefix_len == 127) /* RFC 6164 */
		return;
	ipv6_addr_prefix(&addr, &ifp->addr, ifp->prefix_len);
	if (ipv6_addr_any(&addr))
		return;
	__ipv6_dev_ac_dec(ifp->idev, &addr);
}