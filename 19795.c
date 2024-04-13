static void *single_start(struct seq_file *p, loff_t *pos)
{
	return NULL + (*pos == 0);
}