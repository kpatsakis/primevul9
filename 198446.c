void sc_format_path(const char *str, sc_path_t *path)
{
	int type = SC_PATH_TYPE_PATH;

	if (path) {
		memset(path, 0, sizeof(*path));
		if (*str == 'i' || *str == 'I') {
			type = SC_PATH_TYPE_FILE_ID;
			str++;
		}
		path->len = sizeof(path->value);
		if (sc_hex_to_bin(str, path->value, &path->len) >= 0) {
			path->type = type;
		}
		path->count = -1;
	}
}