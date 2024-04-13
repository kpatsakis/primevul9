static void addrconf_mod_dad_timer(struct inet6_ifaddr *ifp,
				   unsigned long when)
{
	if (!timer_pending(&ifp->dad_timer))
		in6_ifa_hold(ifp);
	mod_timer(&ifp->dad_timer, jiffies + when);
}