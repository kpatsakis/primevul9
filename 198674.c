int sc_path_print(char *buf, size_t buflen, const sc_path_t *path)
{
	size_t i;

	if (buf == NULL || path == NULL)
		return SC_ERROR_INVALID_ARGUMENTS;

	if (buflen < path->len * 2 + path->aid.len * 2 + 1)
		return SC_ERROR_BUFFER_TOO_SMALL;

	buf[0] = '\0';
	if (path->aid.len)   {
		for (i = 0; i < path->aid.len; i++)
			snprintf(buf + strlen(buf), buflen - strlen(buf), "%02x", path->aid.value[i]);
		snprintf(buf + strlen(buf), buflen - strlen(buf), "::");
	}

	for (i = 0; i < path->len; i++)
		snprintf(buf + strlen(buf), buflen - strlen(buf), "%02x", path->value[i]);
	if (!path->aid.len && path->type == SC_PATH_TYPE_DF_NAME)
		snprintf(buf + strlen(buf), buflen - strlen(buf), "::");

	return SC_SUCCESS;
}