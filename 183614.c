static void *igmp_mcf_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct ip_sf_list *psf;
	if (v == SEQ_START_TOKEN)
		psf = igmp_mcf_get_first(seq);
	else
		psf = igmp_mcf_get_next(seq, v);
	++*pos;
	return psf;
}