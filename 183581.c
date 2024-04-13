int ip_mc_source(int add, int omode, struct sock *sk, struct
	ip_mreq_source *mreqs, int ifindex)
{
	int err;
	struct ip_mreqn imr;
	__be32 addr = mreqs->imr_multiaddr;
	struct ip_mc_socklist *pmc;
	struct in_device *in_dev = NULL;
	struct inet_sock *inet = inet_sk(sk);
	struct ip_sf_socklist *psl;
	struct net *net = sock_net(sk);
	int leavegroup = 0;
	int i, j, rv;

	if (!ipv4_is_multicast(addr))
		return -EINVAL;

	ASSERT_RTNL();

	imr.imr_multiaddr.s_addr = mreqs->imr_multiaddr;
	imr.imr_address.s_addr = mreqs->imr_interface;
	imr.imr_ifindex = ifindex;
	in_dev = ip_mc_find_dev(net, &imr);

	if (!in_dev) {
		err = -ENODEV;
		goto done;
	}
	err = -EADDRNOTAVAIL;

	for_each_pmc_rtnl(inet, pmc) {
		if ((pmc->multi.imr_multiaddr.s_addr ==
		     imr.imr_multiaddr.s_addr) &&
		    (pmc->multi.imr_ifindex == imr.imr_ifindex))
			break;
	}
	if (!pmc) {		/* must have a prior join */
		err = -EINVAL;
		goto done;
	}
	/* if a source filter was set, must be the same mode as before */
	if (pmc->sflist) {
		if (pmc->sfmode != omode) {
			err = -EINVAL;
			goto done;
		}
	} else if (pmc->sfmode != omode) {
		/* allow mode switches for empty-set filters */
		ip_mc_add_src(in_dev, &mreqs->imr_multiaddr, omode, 0, NULL, 0);
		ip_mc_del_src(in_dev, &mreqs->imr_multiaddr, pmc->sfmode, 0,
			NULL, 0);
		pmc->sfmode = omode;
	}

	psl = rtnl_dereference(pmc->sflist);
	if (!add) {
		if (!psl)
			goto done;	/* err = -EADDRNOTAVAIL */
		rv = !0;
		for (i = 0; i < psl->sl_count; i++) {
			rv = memcmp(&psl->sl_addr[i], &mreqs->imr_sourceaddr,
				sizeof(__be32));
			if (rv == 0)
				break;
		}
		if (rv)		/* source not found */
			goto done;	/* err = -EADDRNOTAVAIL */

		/* special case - (INCLUDE, empty) == LEAVE_GROUP */
		if (psl->sl_count == 1 && omode == MCAST_INCLUDE) {
			leavegroup = 1;
			goto done;
		}

		/* update the interface filter */
		ip_mc_del_src(in_dev, &mreqs->imr_multiaddr, omode, 1,
			&mreqs->imr_sourceaddr, 1);

		for (j = i+1; j < psl->sl_count; j++)
			psl->sl_addr[j-1] = psl->sl_addr[j];
		psl->sl_count--;
		err = 0;
		goto done;
	}
	/* else, add a new source to the filter */

	if (psl && psl->sl_count >= net->ipv4.sysctl_igmp_max_msf) {
		err = -ENOBUFS;
		goto done;
	}
	if (!psl || psl->sl_count == psl->sl_max) {
		struct ip_sf_socklist *newpsl;
		int count = IP_SFBLOCK;

		if (psl)
			count += psl->sl_max;
		newpsl = sock_kmalloc(sk, IP_SFLSIZE(count), GFP_KERNEL);
		if (!newpsl) {
			err = -ENOBUFS;
			goto done;
		}
		newpsl->sl_max = count;
		newpsl->sl_count = count - IP_SFBLOCK;
		if (psl) {
			for (i = 0; i < psl->sl_count; i++)
				newpsl->sl_addr[i] = psl->sl_addr[i];
			/* decrease mem now to avoid the memleak warning */
			atomic_sub(IP_SFLSIZE(psl->sl_max), &sk->sk_omem_alloc);
			kfree_rcu(psl, rcu);
		}
		rcu_assign_pointer(pmc->sflist, newpsl);
		psl = newpsl;
	}
	rv = 1;	/* > 0 for insert logic below if sl_count is 0 */
	for (i = 0; i < psl->sl_count; i++) {
		rv = memcmp(&psl->sl_addr[i], &mreqs->imr_sourceaddr,
			sizeof(__be32));
		if (rv == 0)
			break;
	}
	if (rv == 0)		/* address already there is an error */
		goto done;
	for (j = psl->sl_count-1; j >= i; j--)
		psl->sl_addr[j+1] = psl->sl_addr[j];
	psl->sl_addr[i] = mreqs->imr_sourceaddr;
	psl->sl_count++;
	err = 0;
	/* update the interface list */
	ip_mc_add_src(in_dev, &mreqs->imr_multiaddr, omode, 1,
		&mreqs->imr_sourceaddr, 1);
done:
	if (leavegroup)
		err = ip_mc_leave_group(sk, &imr);
	return err;
}