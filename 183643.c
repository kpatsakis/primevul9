static int igmp_xmarksources(struct ip_mc_list *pmc, int nsrcs, __be32 *srcs)
{
	struct ip_sf_list *psf;
	int i, scount;

	scount = 0;
	for (psf = pmc->sources; psf; psf = psf->sf_next) {
		if (scount == nsrcs)
			break;
		for (i = 0; i < nsrcs; i++) {
			/* skip inactive filters */
			if (psf->sf_count[MCAST_INCLUDE] ||
			    pmc->sfcount[MCAST_EXCLUDE] !=
			    psf->sf_count[MCAST_EXCLUDE])
				break;
			if (srcs[i] == psf->sf_inaddr) {
				scount++;
				break;
			}
		}
	}
	pmc->gsquery = 0;
	if (scount == nsrcs)	/* all sources excluded */
		return 0;
	return 1;
}