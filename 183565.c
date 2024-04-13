static struct ip_sf_list *igmp_mcf_get_idx(struct seq_file *seq, loff_t pos)
{
	struct ip_sf_list *psf = igmp_mcf_get_first(seq);
	if (psf)
		while (pos && (psf = igmp_mcf_get_next(seq, psf)) != NULL)
			--pos;
	return pos ? NULL : psf;
}