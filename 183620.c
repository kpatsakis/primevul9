int ip_mc_msfilter(struct sock *sk, struct ip_msfilter *msf, int ifindex)
{
	int err = 0;
	struct ip_mreqn	imr;
	__be32 addr = msf->imsf_multiaddr;
	struct ip_mc_socklist *pmc;
	struct in_device *in_dev;
	struct inet_sock *inet = inet_sk(sk);
	struct ip_sf_socklist *newpsl, *psl;
	struct net *net = sock_net(sk);
	int leavegroup = 0;

	if (!ipv4_is_multicast(addr))
		return -EINVAL;
	if (msf->imsf_fmode != MCAST_INCLUDE &&
	    msf->imsf_fmode != MCAST_EXCLUDE)
		return -EINVAL;

	ASSERT_RTNL();

	imr.imr_multiaddr.s_addr = msf->imsf_multiaddr;
	imr.imr_address.s_addr = msf->imsf_interface;
	imr.imr_ifindex = ifindex;
	in_dev = ip_mc_find_dev(net, &imr);

	if (!in_dev) {
		err = -ENODEV;
		goto done;
	}

	/* special case - (INCLUDE, empty) == LEAVE_GROUP */
	if (msf->imsf_fmode == MCAST_INCLUDE && msf->imsf_numsrc == 0) {
		leavegroup = 1;
		goto done;
	}

	for_each_pmc_rtnl(inet, pmc) {
		if (pmc->multi.imr_multiaddr.s_addr == msf->imsf_multiaddr &&
		    pmc->multi.imr_ifindex == imr.imr_ifindex)
			break;
	}
	if (!pmc) {		/* must have a prior join */
		err = -EINVAL;
		goto done;
	}
	if (msf->imsf_numsrc) {
		newpsl = sock_kmalloc(sk, IP_SFLSIZE(msf->imsf_numsrc),
							   GFP_KERNEL);
		if (!newpsl) {
			err = -ENOBUFS;
			goto done;
		}
		newpsl->sl_max = newpsl->sl_count = msf->imsf_numsrc;
		memcpy(newpsl->sl_addr, msf->imsf_slist,
			msf->imsf_numsrc * sizeof(msf->imsf_slist[0]));
		err = ip_mc_add_src(in_dev, &msf->imsf_multiaddr,
			msf->imsf_fmode, newpsl->sl_count, newpsl->sl_addr, 0);
		if (err) {
			sock_kfree_s(sk, newpsl, IP_SFLSIZE(newpsl->sl_max));
			goto done;
		}
	} else {
		newpsl = NULL;
		(void) ip_mc_add_src(in_dev, &msf->imsf_multiaddr,
				     msf->imsf_fmode, 0, NULL, 0);
	}
	psl = rtnl_dereference(pmc->sflist);
	if (psl) {
		(void) ip_mc_del_src(in_dev, &msf->imsf_multiaddr, pmc->sfmode,
			psl->sl_count, psl->sl_addr, 0);
		/* decrease mem now to avoid the memleak warning */
		atomic_sub(IP_SFLSIZE(psl->sl_max), &sk->sk_omem_alloc);
		kfree_rcu(psl, rcu);
	} else
		(void) ip_mc_del_src(in_dev, &msf->imsf_multiaddr, pmc->sfmode,
			0, NULL, 0);
	rcu_assign_pointer(pmc->sflist, newpsl);
	pmc->sfmode = msf->imsf_fmode;
	err = 0;
done:
	if (leavegroup)
		err = ip_mc_leave_group(sk, &imr);
	return err;
}