PS_DESTROY_FUNC(files)
{
	char buf[MAXPATHLEN];
	PS_FILES_DATA;

	if (!ps_files_path_create(buf, sizeof(buf), data, key)) {
		return FAILURE;
	}

	if (data->fd != -1) {
		ps_files_close(data);

		if (VCWD_UNLINK(buf) == -1) {
			/* This is a little safety check for instances when we are dealing with a regenerated session
			 * that was not yet written to disk. */
			if (!VCWD_ACCESS(buf, F_OK)) {
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}