static void *seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	return get_next(s, v);
}