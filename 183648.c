static int igmp_marksources(struct ip_mc_list *pmc, int nsrcs, __be32 *srcs)
{
	struct ip_sf_list *psf;
	int i, scount;

	if (pmc->sfmode == MCAST_EXCLUDE)
		return igmp_xmarksources(pmc, nsrcs, srcs);

	/* mark INCLUDE-mode sources */
	scount = 0;
	for (psf = pmc->sources; psf; psf = psf->sf_next) {
		if (scount == nsrcs)
			break;
		for (i = 0; i < nsrcs; i++)
			if (srcs[i] == psf->sf_inaddr) {
				psf->sf_gsresp = 1;
				scount++;
				break;
			}
	}
	if (!scount) {
		pmc->gsquery = 0;
		return 0;
	}
	pmc->gsquery = 1;
	return 1;
}