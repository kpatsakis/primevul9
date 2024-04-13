static int ip_mc_leave_src(struct sock *sk, struct ip_mc_socklist *iml,
			   struct in_device *in_dev)
{
	struct ip_sf_socklist *psf = rtnl_dereference(iml->sflist);
	int err;

	if (!psf) {
		/* any-source empty exclude case */
		return ip_mc_del_src(in_dev, &iml->multi.imr_multiaddr.s_addr,
			iml->sfmode, 0, NULL, 0);
	}
	err = ip_mc_del_src(in_dev, &iml->multi.imr_multiaddr.s_addr,
			iml->sfmode, psf->sl_count, psf->sl_addr, 0);
	RCU_INIT_POINTER(iml->sflist, NULL);
	/* decrease mem now to avoid the memleak warning */
	atomic_sub(IP_SFLSIZE(psf->sl_max), &sk->sk_omem_alloc);
	kfree_rcu(psf, rcu);
	return err;
}