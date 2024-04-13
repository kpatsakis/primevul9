static void *igmp_mc_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct ip_mc_list *im;
	if (v == SEQ_START_TOKEN)
		im = igmp_mc_get_first(seq);
	else
		im = igmp_mc_get_next(seq, v);
	++*pos;
	return im;
}