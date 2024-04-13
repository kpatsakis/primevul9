void seq_put_decimal_ull_width(struct seq_file *m, const char *delimiter,
			 unsigned long long num, unsigned int width)
{
	int len;

	if (m->count + 2 >= m->size) /* we'll write 2 bytes at least */
		goto overflow;

	if (delimiter && delimiter[0]) {
		if (delimiter[1] == 0)
			seq_putc(m, delimiter[0]);
		else
			seq_puts(m, delimiter);
	}

	if (!width)
		width = 1;

	if (m->count + width >= m->size)
		goto overflow;

	len = num_to_str(m->buf + m->count, m->size - m->count, num, width);
	if (!len)
		goto overflow;

	m->count += len;
	return;

overflow:
	seq_set_overflow(m);
}