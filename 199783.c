static void ipv6_ifa_notify(int event, struct inet6_ifaddr *ifp)
{
	rcu_read_lock_bh();
	if (likely(ifp->idev->dead == 0))
		__ipv6_ifa_notify(event, ifp);
	rcu_read_unlock_bh();
}