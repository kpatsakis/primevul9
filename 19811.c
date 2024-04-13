void seq_escape_mem(struct seq_file *m, const char *src, size_t len,
		    unsigned int flags, const char *esc)
{
	char *buf;
	size_t size = seq_get_buf(m, &buf);
	int ret;

	ret = string_escape_mem(src, len, buf, size, flags, esc);
	seq_commit(m, ret < size ? ret : -1);
}