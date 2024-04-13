int sc_compare_path(const sc_path_t *path1, const sc_path_t *path2)
{
	return path1->len == path2->len
		&& !memcmp(path1->value, path2->value, path1->len);
}