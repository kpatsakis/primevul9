int sc_compare_path_prefix(const sc_path_t *prefix, const sc_path_t *path)
{
	sc_path_t tpath;

	if (prefix->len > path->len)
		return 0;

	tpath     = *path;
	tpath.len = prefix->len;

	return sc_compare_path(&tpath, prefix);
}