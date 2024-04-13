void seq_put_decimal_ll(struct seq_file *m, const char *delimiter, long long num)
{
	int len;

	if (m->count + 3 >= m->size) /* we'll write 2 bytes at least */
		goto overflow;

	if (delimiter && delimiter[0]) {
		if (delimiter[1] == 0)
			seq_putc(m, delimiter[0]);
		else
			seq_puts(m, delimiter);
	}

	if (m->count + 2 >= m->size)
		goto overflow;

	if (num < 0) {
		m->buf[m->count++] = '-';
		num = -num;
	}

	if (num < 10) {
		m->buf[m->count++] = num + '0';
		return;
	}

	len = num_to_str(m->buf + m->count, m->size - m->count, num, 0);
	if (!len)
		goto overflow;

	m->count += len;
	return;

overflow:
	seq_set_overflow(m);
}