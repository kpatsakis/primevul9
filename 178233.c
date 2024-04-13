const char *prefix_filename(const char *pfx, int pfx_len, const char *arg)
{
	static char path[PATH_MAX];
#ifndef WIN32
	if (!pfx || !*pfx || is_absolute_path(arg))
		return arg;
	memcpy(path, pfx, pfx_len);
	strcpy(path + pfx_len, arg);
#else
	char *p;
	/* don't add prefix to absolute paths, but still replace '\' by '/' */
	if (is_absolute_path(arg))
		pfx_len = 0;
	else
		memcpy(path, pfx, pfx_len);
	strcpy(path + pfx_len, arg);
	for (p = path + pfx_len; *p; p++)
		if (*p == '\\')
			*p = '/';
#endif
	return path;
}