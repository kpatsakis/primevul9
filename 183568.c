static int ip_mc_add1_src(struct ip_mc_list *pmc, int sfmode,
	__be32 *psfsrc)
{
	struct ip_sf_list *psf, *psf_prev;

	psf_prev = NULL;
	for (psf = pmc->sources; psf; psf = psf->sf_next) {
		if (psf->sf_inaddr == *psfsrc)
			break;
		psf_prev = psf;
	}
	if (!psf) {
		psf = kzalloc(sizeof(*psf), GFP_ATOMIC);
		if (!psf)
			return -ENOBUFS;
		psf->sf_inaddr = *psfsrc;
		if (psf_prev) {
			psf_prev->sf_next = psf;
		} else
			pmc->sources = psf;
	}
	psf->sf_count[sfmode]++;
	if (psf->sf_count[sfmode] == 1) {
		ip_rt_multicast_event(pmc->interface);
	}
	return 0;
}