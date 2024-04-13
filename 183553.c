static void sf_markstate(struct ip_mc_list *pmc)
{
	struct ip_sf_list *psf;
	int mca_xcount = pmc->sfcount[MCAST_EXCLUDE];

	for (psf = pmc->sources; psf; psf = psf->sf_next)
		if (pmc->sfcount[MCAST_EXCLUDE]) {
			psf->sf_oldin = mca_xcount ==
				psf->sf_count[MCAST_EXCLUDE] &&
				!psf->sf_count[MCAST_INCLUDE];
		} else
			psf->sf_oldin = psf->sf_count[MCAST_INCLUDE] != 0;
}