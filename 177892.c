static char *ps_files_path_create(char *buf, size_t buflen, ps_files *data, const char *key)
{
	size_t key_len;
	const char *p;
	int i;
	int n;

	key_len = strlen(key);
	if (key_len <= data->dirdepth ||
		buflen < (strlen(data->basedir) + 2 * data->dirdepth + key_len + 5 + sizeof(FILE_PREFIX))) {
		return NULL;
	}

	p = key;
	memcpy(buf, data->basedir, data->basedir_len);
	n = data->basedir_len;
	buf[n++] = PHP_DIR_SEPARATOR;
	for (i = 0; i < (int)data->dirdepth; i++) {
		buf[n++] = *p++;
		buf[n++] = PHP_DIR_SEPARATOR;
	}
	memcpy(buf + n, FILE_PREFIX, sizeof(FILE_PREFIX) - 1);
	n += sizeof(FILE_PREFIX) - 1;
	memcpy(buf + n, key, key_len);
	n += key_len;
	buf[n] = '\0';

	return buf;
}