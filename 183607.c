static int ip_mc_del1_src(struct ip_mc_list *pmc, int sfmode,
	__be32 *psfsrc)
{
	struct ip_sf_list *psf, *psf_prev;
	int rv = 0;

	psf_prev = NULL;
	for (psf = pmc->sources; psf; psf = psf->sf_next) {
		if (psf->sf_inaddr == *psfsrc)
			break;
		psf_prev = psf;
	}
	if (!psf || psf->sf_count[sfmode] == 0) {
		/* source filter not found, or count wrong =>  bug */
		return -ESRCH;
	}
	psf->sf_count[sfmode]--;
	if (psf->sf_count[sfmode] == 0) {
		ip_rt_multicast_event(pmc->interface);
	}
	if (!psf->sf_count[MCAST_INCLUDE] && !psf->sf_count[MCAST_EXCLUDE]) {
#ifdef CONFIG_IP_MULTICAST
		struct in_device *in_dev = pmc->interface;
		struct net *net = dev_net(in_dev->dev);
#endif

		/* no more filters for this source */
		if (psf_prev)
			psf_prev->sf_next = psf->sf_next;
		else
			pmc->sources = psf->sf_next;
#ifdef CONFIG_IP_MULTICAST
		if (psf->sf_oldin &&
		    !IGMP_V1_SEEN(in_dev) && !IGMP_V2_SEEN(in_dev)) {
			psf->sf_crcount = in_dev->mr_qrv ?: net->ipv4.sysctl_igmp_qrv;
			psf->sf_next = pmc->tomb;
			pmc->tomb = psf;
			rv = 1;
		} else
#endif
			kfree(psf);
	}
	return rv;
}