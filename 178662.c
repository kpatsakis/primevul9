static char *sanitize_path(const char *path)
{
	char *p;

	if (!path)
		return NULL;

	p = canonicalize_path_restricted(path);
	if (!p)
		err(MOUNT_EX_USAGE, "%s", path);

	return p;
}