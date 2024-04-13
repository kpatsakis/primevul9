int seq_path_root(struct seq_file *m, const struct path *path,
		  const struct path *root, const char *esc)
{
	char *buf;
	size_t size = seq_get_buf(m, &buf);
	int res = -ENAMETOOLONG;

	if (size) {
		char *p;

		p = __d_path(path, root, buf, size);
		if (!p)
			return SEQ_SKIP;
		res = PTR_ERR(p);
		if (!IS_ERR(p)) {
			char *end = mangle_path(buf, p, esc);
			if (end)
				res = end - buf;
			else
				res = -ENAMETOOLONG;
		}
	}
	seq_commit(m, res);

	return res < 0 && res != -ENAMETOOLONG ? res : 0;
}