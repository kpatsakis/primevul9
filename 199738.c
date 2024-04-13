static void addrconf_del_dad_timer(struct inet6_ifaddr *ifp)
{
	if (del_timer(&ifp->dad_timer))
		__in6_ifa_put(ifp);
}