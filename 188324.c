int git_path_root(const char *path)
{
	int offset = 0;

	/* Does the root of the path look like a windows drive ? */
	if (LOOKS_LIKE_DRIVE_PREFIX(path))
		offset += 2;

#ifdef GIT_WIN32
	/* Are we dealing with a windows network path? */
	else if ((path[0] == '/' && path[1] == '/' && path[2] != '/') ||
		(path[0] == '\\' && path[1] == '\\' && path[2] != '\\'))
	{
		offset += 2;

		/* Skip the computer name segment */
		while (path[offset] && path[offset] != '/' && path[offset] != '\\')
			offset++;
	}

	if (path[offset] == '\\')
		return offset;
#endif

	if (path[offset] == '/')
		return offset;

	return -1;	/* Not a real error - signals that path is not rooted */
}