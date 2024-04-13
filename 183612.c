static int sf_setstate(struct ip_mc_list *pmc)
{
	struct ip_sf_list *psf, *dpsf;
	int mca_xcount = pmc->sfcount[MCAST_EXCLUDE];
	int qrv = pmc->interface->mr_qrv;
	int new_in, rv;

	rv = 0;
	for (psf = pmc->sources; psf; psf = psf->sf_next) {
		if (pmc->sfcount[MCAST_EXCLUDE]) {
			new_in = mca_xcount == psf->sf_count[MCAST_EXCLUDE] &&
				!psf->sf_count[MCAST_INCLUDE];
		} else
			new_in = psf->sf_count[MCAST_INCLUDE] != 0;
		if (new_in) {
			if (!psf->sf_oldin) {
				struct ip_sf_list *prev = NULL;

				for (dpsf = pmc->tomb; dpsf; dpsf = dpsf->sf_next) {
					if (dpsf->sf_inaddr == psf->sf_inaddr)
						break;
					prev = dpsf;
				}
				if (dpsf) {
					if (prev)
						prev->sf_next = dpsf->sf_next;
					else
						pmc->tomb = dpsf->sf_next;
					kfree(dpsf);
				}
				psf->sf_crcount = qrv;
				rv++;
			}
		} else if (psf->sf_oldin) {

			psf->sf_crcount = 0;
			/*
			 * add or update "delete" records if an active filter
			 * is now inactive
			 */
			for (dpsf = pmc->tomb; dpsf; dpsf = dpsf->sf_next)
				if (dpsf->sf_inaddr == psf->sf_inaddr)
					break;
			if (!dpsf) {
				dpsf = kmalloc(sizeof(*dpsf), GFP_ATOMIC);
				if (!dpsf)
					continue;
				*dpsf = *psf;
				/* pmc->lock held by callers */
				dpsf->sf_next = pmc->tomb;
				pmc->tomb = dpsf;
			}
			dpsf->sf_crcount = qrv;
			rv++;
		}
	}
	return rv;
}