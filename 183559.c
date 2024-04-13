igmp_scount(struct ip_mc_list *pmc, int type, int gdeleted, int sdeleted)
{
	struct ip_sf_list *psf;
	int scount = 0;

	for (psf = pmc->sources; psf; psf = psf->sf_next) {
		if (!is_in(pmc, psf, type, gdeleted, sdeleted))
			continue;
		scount++;
	}
	return scount;
}