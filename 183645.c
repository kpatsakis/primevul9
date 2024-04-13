static struct ip_mc_list *igmp_mc_get_idx(struct seq_file *seq, loff_t pos)
{
	struct ip_mc_list *im = igmp_mc_get_first(seq);
	if (im)
		while (pos && (im = igmp_mc_get_next(seq, im)) != NULL)
			--pos;
	return pos ? NULL : im;
}