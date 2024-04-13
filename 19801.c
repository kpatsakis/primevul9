void seq_bprintf(struct seq_file *m, const char *f, const u32 *binary)
{
	int len;

	if (m->count < m->size) {
		len = bstr_printf(m->buf + m->count, m->size - m->count, f,
				  binary);
		if (m->count + len < m->size) {
			m->count += len;
			return;
		}
	}
	seq_set_overflow(m);
}