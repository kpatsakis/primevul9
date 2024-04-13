static void *single_next(struct seq_file *p, void *v, loff_t *pos)
{
	++*pos;
	return NULL;
}