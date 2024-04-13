static void *yam_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	++*pos;
	return (*pos < NR_PORTS) ? yam_devs[*pos] : NULL;
}