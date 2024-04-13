static int is_in(struct ip_mc_list *pmc, struct ip_sf_list *psf, int type,
	int gdeleted, int sdeleted)
{
	switch (type) {
	case IGMPV3_MODE_IS_INCLUDE:
	case IGMPV3_MODE_IS_EXCLUDE:
		if (gdeleted || sdeleted)
			return 0;
		if (!(pmc->gsquery && !psf->sf_gsresp)) {
			if (pmc->sfmode == MCAST_INCLUDE)
				return 1;
			/* don't include if this source is excluded
			 * in all filters
			 */
			if (psf->sf_count[MCAST_INCLUDE])
				return type == IGMPV3_MODE_IS_INCLUDE;
			return pmc->sfcount[MCAST_EXCLUDE] ==
				psf->sf_count[MCAST_EXCLUDE];
		}
		return 0;
	case IGMPV3_CHANGE_TO_INCLUDE:
		if (gdeleted || sdeleted)
			return 0;
		return psf->sf_count[MCAST_INCLUDE] != 0;
	case IGMPV3_CHANGE_TO_EXCLUDE:
		if (gdeleted || sdeleted)
			return 0;
		if (pmc->sfcount[MCAST_EXCLUDE] == 0 ||
		    psf->sf_count[MCAST_INCLUDE])
			return 0;
		return pmc->sfcount[MCAST_EXCLUDE] ==
			psf->sf_count[MCAST_EXCLUDE];
	case IGMPV3_ALLOW_NEW_SOURCES:
		if (gdeleted || !psf->sf_crcount)
			return 0;
		return (pmc->sfmode == MCAST_INCLUDE) ^ sdeleted;
	case IGMPV3_BLOCK_OLD_SOURCES:
		if (pmc->sfmode == MCAST_INCLUDE)
			return gdeleted || (psf->sf_crcount && sdeleted);
		return psf->sf_crcount && !gdeleted && !sdeleted;
	}
	return 0;
}