int ip_mc_msfget(struct sock *sk, struct ip_msfilter *msf,
	struct ip_msfilter __user *optval, int __user *optlen)
{
	int err, len, count, copycount;
	struct ip_mreqn	imr;
	__be32 addr = msf->imsf_multiaddr;
	struct ip_mc_socklist *pmc;
	struct in_device *in_dev;
	struct inet_sock *inet = inet_sk(sk);
	struct ip_sf_socklist *psl;
	struct net *net = sock_net(sk);

	ASSERT_RTNL();

	if (!ipv4_is_multicast(addr))
		return -EINVAL;

	imr.imr_multiaddr.s_addr = msf->imsf_multiaddr;
	imr.imr_address.s_addr = msf->imsf_interface;
	imr.imr_ifindex = 0;
	in_dev = ip_mc_find_dev(net, &imr);

	if (!in_dev) {
		err = -ENODEV;
		goto done;
	}
	err = -EADDRNOTAVAIL;

	for_each_pmc_rtnl(inet, pmc) {
		if (pmc->multi.imr_multiaddr.s_addr == msf->imsf_multiaddr &&
		    pmc->multi.imr_ifindex == imr.imr_ifindex)
			break;
	}
	if (!pmc)		/* must have a prior join */
		goto done;
	msf->imsf_fmode = pmc->sfmode;
	psl = rtnl_dereference(pmc->sflist);
	if (!psl) {
		len = 0;
		count = 0;
	} else {
		count = psl->sl_count;
	}
	copycount = count < msf->imsf_numsrc ? count : msf->imsf_numsrc;
	len = copycount * sizeof(psl->sl_addr[0]);
	msf->imsf_numsrc = count;
	if (put_user(IP_MSFILTER_SIZE(copycount), optlen) ||
	    copy_to_user(optval, msf, IP_MSFILTER_SIZE(0))) {
		return -EFAULT;
	}
	if (len &&
	    copy_to_user(&optval->imsf_slist[0], psl->sl_addr, len))
		return -EFAULT;
	return 0;
done:
	return err;
}