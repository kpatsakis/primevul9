static void igmpv3_del_delrec(struct in_device *in_dev, struct ip_mc_list *im)
{
	struct ip_mc_list *pmc, *pmc_prev;
	struct ip_sf_list *psf;
	struct net *net = dev_net(in_dev->dev);
	__be32 multiaddr = im->multiaddr;

	spin_lock_bh(&in_dev->mc_tomb_lock);
	pmc_prev = NULL;
	for (pmc = in_dev->mc_tomb; pmc; pmc = pmc->next) {
		if (pmc->multiaddr == multiaddr)
			break;
		pmc_prev = pmc;
	}
	if (pmc) {
		if (pmc_prev)
			pmc_prev->next = pmc->next;
		else
			in_dev->mc_tomb = pmc->next;
	}
	spin_unlock_bh(&in_dev->mc_tomb_lock);

	spin_lock_bh(&im->lock);
	if (pmc) {
		im->interface = pmc->interface;
		if (im->sfmode == MCAST_INCLUDE) {
			swap(im->tomb, pmc->tomb);
			swap(im->sources, pmc->sources);
			for (psf = im->sources; psf; psf = psf->sf_next)
				psf->sf_crcount = in_dev->mr_qrv ?: net->ipv4.sysctl_igmp_qrv;
		} else {
			im->crcount = in_dev->mr_qrv ?: net->ipv4.sysctl_igmp_qrv;
		}
		in_dev_put(pmc->interface);
		kfree_pmc(pmc);
	}
	spin_unlock_bh(&im->lock);
}