int seq_path(struct seq_file *m, const struct path *path, const char *esc)
{
	char *buf;
	size_t size = seq_get_buf(m, &buf);
	int res = -1;

	if (size) {
		char *p = d_path(path, buf, size);
		if (!IS_ERR(p)) {
			char *end = mangle_path(buf, p, esc);
			if (end)
				res = end - buf;
		}
	}
	seq_commit(m, res);

	return res;
}