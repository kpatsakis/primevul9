int kern_path(const char *name, unsigned int flags, struct path *path)
{
	struct nameidata nd;
	int res = do_path_lookup(AT_FDCWD, name, flags, &nd);
	if (!res)
		*path = nd.path;
	return res;
}