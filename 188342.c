bool git_path_isvalid(
	git_repository *repo,
	const char *path,
	uint16_t mode,
	unsigned int flags)
{
	const char *start, *c;

	/* Upgrade the ".git" checks based on platform */
	if ((flags & GIT_PATH_REJECT_DOT_GIT))
		flags = dotgit_flags(repo, flags);

	for (start = c = path; *c; c++) {
		if (!verify_char(*c, flags))
			return false;

		if (*c == '/') {
			if (!verify_component(repo, start, (c - start), mode, flags))
				return false;

			start = c+1;
		}
	}

	return verify_component(repo, start, (c - start), mode, flags);
}