int sc_path_set(sc_path_t *path, int type, const u8 *id, size_t id_len,
	int idx, int count)
{
	if (path == NULL || id == NULL || id_len == 0 || id_len > SC_MAX_PATH_SIZE)
		return SC_ERROR_INVALID_ARGUMENTS;

	memset(path, 0, sizeof(*path));
	memcpy(path->value, id, id_len);
	path->len   = id_len;
	path->type  = type;
	path->index = idx;
	path->count = count;

	return SC_SUCCESS;
}