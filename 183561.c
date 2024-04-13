static void ip_sf_list_clear_all(struct ip_sf_list *psf)
{
	struct ip_sf_list *next;

	while (psf) {
		next = psf->sf_next;
		kfree(psf);
		psf = next;
	}
}