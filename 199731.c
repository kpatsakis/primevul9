static void addrconf_dad_kick(struct inet6_ifaddr *ifp)
{
	unsigned long rand_num;
	struct inet6_dev *idev = ifp->idev;

	if (ifp->flags & IFA_F_OPTIMISTIC)
		rand_num = 0;
	else
		rand_num = net_random() % (idev->cnf.rtr_solicit_delay ? : 1);

	ifp->dad_probes = idev->cnf.dad_transmits;
	addrconf_mod_dad_timer(ifp, rand_num);
}