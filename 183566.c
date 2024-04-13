static void igmpv3_clear_zeros(struct ip_sf_list **ppsf)
{
	struct ip_sf_list *psf_prev, *psf_next, *psf;

	psf_prev = NULL;
	for (psf = *ppsf; psf; psf = psf_next) {
		psf_next = psf->sf_next;
		if (psf->sf_crcount == 0) {
			if (psf_prev)
				psf_prev->sf_next = psf->sf_next;
			else
				*ppsf = psf->sf_next;
			kfree(psf);
		} else
			psf_prev = psf;
	}
}