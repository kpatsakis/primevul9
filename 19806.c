void seq_putc(struct seq_file *m, char c)
{
	if (m->count >= m->size)
		return;

	m->buf[m->count++] = c;
}