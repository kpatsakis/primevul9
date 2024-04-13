void seq_pad(struct seq_file *m, char c)
{
	int size = m->pad_until - m->count;
	if (size > 0) {
		if (size + m->count > m->size) {
			seq_set_overflow(m);
			return;
		}
		memset(m->buf + m->count, ' ', size);
		m->count += size;
	}
	if (c)
		seq_putc(m, c);
}