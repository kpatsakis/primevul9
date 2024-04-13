static int __ip_mc_join_group(struct sock *sk, struct ip_mreqn *imr,
			      unsigned int mode)
{
	__be32 addr = imr->imr_multiaddr.s_addr;
	struct ip_mc_socklist *iml, *i;
	struct in_device *in_dev;
	struct inet_sock *inet = inet_sk(sk);
	struct net *net = sock_net(sk);
	int ifindex;
	int count = 0;
	int err;

	ASSERT_RTNL();

	if (!ipv4_is_multicast(addr))
		return -EINVAL;

	in_dev = ip_mc_find_dev(net, imr);

	if (!in_dev) {
		err = -ENODEV;
		goto done;
	}

	err = -EADDRINUSE;
	ifindex = imr->imr_ifindex;
	for_each_pmc_rtnl(inet, i) {
		if (i->multi.imr_multiaddr.s_addr == addr &&
		    i->multi.imr_ifindex == ifindex)
			goto done;
		count++;
	}
	err = -ENOBUFS;
	if (count >= net->ipv4.sysctl_igmp_max_memberships)
		goto done;
	iml = sock_kmalloc(sk, sizeof(*iml), GFP_KERNEL);
	if (!iml)
		goto done;

	memcpy(&iml->multi, imr, sizeof(*imr));
	iml->next_rcu = inet->mc_list;
	iml->sflist = NULL;
	iml->sfmode = mode;
	rcu_assign_pointer(inet->mc_list, iml);
	____ip_mc_inc_group(in_dev, addr, mode, GFP_KERNEL);
	err = 0;
done:
	return err;
}