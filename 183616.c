static int ip_mc_add_src(struct in_device *in_dev, __be32 *pmca, int sfmode,
			 int sfcount, __be32 *psfsrc, int delta)
{
	struct ip_mc_list *pmc;
	int	isexclude;
	int	i, err;

	if (!in_dev)
		return -ENODEV;
	rcu_read_lock();
	for_each_pmc_rcu(in_dev, pmc) {
		if (*pmca == pmc->multiaddr)
			break;
	}
	if (!pmc) {
		/* MCA not found?? bug */
		rcu_read_unlock();
		return -ESRCH;
	}
	spin_lock_bh(&pmc->lock);
	rcu_read_unlock();

#ifdef CONFIG_IP_MULTICAST
	sf_markstate(pmc);
#endif
	isexclude = pmc->sfmode == MCAST_EXCLUDE;
	if (!delta)
		pmc->sfcount[sfmode]++;
	err = 0;
	for (i = 0; i < sfcount; i++) {
		err = ip_mc_add1_src(pmc, sfmode, &psfsrc[i]);
		if (err)
			break;
	}
	if (err) {
		int j;

		if (!delta)
			pmc->sfcount[sfmode]--;
		for (j = 0; j < i; j++)
			(void) ip_mc_del1_src(pmc, sfmode, &psfsrc[j]);
	} else if (isexclude != (pmc->sfcount[MCAST_EXCLUDE] != 0)) {
#ifdef CONFIG_IP_MULTICAST
		struct ip_sf_list *psf;
		struct net *net = dev_net(pmc->interface->dev);
		in_dev = pmc->interface;
#endif

		/* filter mode change */
		if (pmc->sfcount[MCAST_EXCLUDE])
			pmc->sfmode = MCAST_EXCLUDE;
		else if (pmc->sfcount[MCAST_INCLUDE])
			pmc->sfmode = MCAST_INCLUDE;
#ifdef CONFIG_IP_MULTICAST
		/* else no filters; keep old mode for reports */

		pmc->crcount = in_dev->mr_qrv ?: net->ipv4.sysctl_igmp_qrv;
		in_dev->mr_ifc_count = pmc->crcount;
		for (psf = pmc->sources; psf; psf = psf->sf_next)
			psf->sf_crcount = 0;
		igmp_ifc_event(in_dev);
	} else if (sf_setstate(pmc)) {
		igmp_ifc_event(in_dev);
#endif
	}
	spin_unlock_bh(&pmc->lock);
	return err;
}