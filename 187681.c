static void *yam_seq_start(struct seq_file *seq, loff_t *pos)
{
	return (*pos < NR_PORTS) ? yam_devs[*pos] : NULL;
}