int ip_mc_gsfget(struct sock *sk, struct group_filter *gsf,
	struct sockaddr_storage __user *p)
{
	int i, count, copycount;
	struct sockaddr_in *psin;
	__be32 addr;
	struct ip_mc_socklist *pmc;
	struct inet_sock *inet = inet_sk(sk);
	struct ip_sf_socklist *psl;

	ASSERT_RTNL();

	psin = (struct sockaddr_in *)&gsf->gf_group;
	if (psin->sin_family != AF_INET)
		return -EINVAL;
	addr = psin->sin_addr.s_addr;
	if (!ipv4_is_multicast(addr))
		return -EINVAL;

	for_each_pmc_rtnl(inet, pmc) {
		if (pmc->multi.imr_multiaddr.s_addr == addr &&
		    pmc->multi.imr_ifindex == gsf->gf_interface)
			break;
	}
	if (!pmc)		/* must have a prior join */
		return -EADDRNOTAVAIL;
	gsf->gf_fmode = pmc->sfmode;
	psl = rtnl_dereference(pmc->sflist);
	count = psl ? psl->sl_count : 0;
	copycount = count < gsf->gf_numsrc ? count : gsf->gf_numsrc;
	gsf->gf_numsrc = count;
	for (i = 0; i < copycount; i++, p++) {
		struct sockaddr_storage ss;

		psin = (struct sockaddr_in *)&ss;
		memset(&ss, 0, sizeof(ss));
		psin->sin_family = AF_INET;
		psin->sin_addr.s_addr = psl->sl_addr[i];
		if (copy_to_user(p, &ss, sizeof(ss)))
			return -EFAULT;
	}
	return 0;
}