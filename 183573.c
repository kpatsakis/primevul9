static int ip_mc_del_src(struct in_device *in_dev, __be32 *pmca, int sfmode,
			 int sfcount, __be32 *psfsrc, int delta)
{
	struct ip_mc_list *pmc;
	int	changerec = 0;
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
	if (!delta) {
		err = -EINVAL;
		if (!pmc->sfcount[sfmode])
			goto out_unlock;
		pmc->sfcount[sfmode]--;
	}
	err = 0;
	for (i = 0; i < sfcount; i++) {
		int rv = ip_mc_del1_src(pmc, sfmode, &psfsrc[i]);

		changerec |= rv > 0;
		if (!err && rv < 0)
			err = rv;
	}
	if (pmc->sfmode == MCAST_EXCLUDE &&
	    pmc->sfcount[MCAST_EXCLUDE] == 0 &&
	    pmc->sfcount[MCAST_INCLUDE]) {
#ifdef CONFIG_IP_MULTICAST
		struct ip_sf_list *psf;
		struct net *net = dev_net(in_dev->dev);
#endif

		/* filter mode change */
		pmc->sfmode = MCAST_INCLUDE;
#ifdef CONFIG_IP_MULTICAST
		pmc->crcount = in_dev->mr_qrv ?: net->ipv4.sysctl_igmp_qrv;
		in_dev->mr_ifc_count = pmc->crcount;
		for (psf = pmc->sources; psf; psf = psf->sf_next)
			psf->sf_crcount = 0;
		igmp_ifc_event(pmc->interface);
	} else if (sf_setstate(pmc) || changerec) {
		igmp_ifc_event(pmc->interface);
#endif
	}
out_unlock:
	spin_unlock_bh(&pmc->lock);
	return err;
}