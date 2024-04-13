static bool verify_component(
	git_repository *repo,
	const char *component,
	size_t len,
	uint16_t mode,
	unsigned int flags)
{
	if (len == 0)
		return false;

	if ((flags & GIT_PATH_REJECT_TRAVERSAL) &&
		len == 1 && component[0] == '.')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAVERSAL) &&
		len == 2 && component[0] == '.' && component[1] == '.')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAILING_DOT) && component[len-1] == '.')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAILING_SPACE) && component[len-1] == ' ')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAILING_COLON) && component[len-1] == ':')
		return false;

	if (flags & GIT_PATH_REJECT_DOS_PATHS) {
		if (!verify_dospath(component, len, "CON", false) ||
			!verify_dospath(component, len, "PRN", false) ||
			!verify_dospath(component, len, "AUX", false) ||
			!verify_dospath(component, len, "NUL", false) ||
			!verify_dospath(component, len, "COM", true)  ||
			!verify_dospath(component, len, "LPT", true))
			return false;
	}

	if (flags & GIT_PATH_REJECT_DOT_GIT_HFS) {
		if (!verify_dotgit_hfs(component, len))
			return false;
		if (S_ISLNK(mode) && git_path_is_gitfile(component, len, GIT_PATH_GITFILE_GITMODULES, GIT_PATH_FS_HFS))
			return false;
	}

	if (flags & GIT_PATH_REJECT_DOT_GIT_NTFS) {
		if (!verify_dotgit_ntfs(repo, component, len))
			return false;
		if (S_ISLNK(mode) && git_path_is_gitfile(component, len, GIT_PATH_GITFILE_GITMODULES, GIT_PATH_FS_NTFS))
			return false;
	}

	/* don't bother rerunning the `.git` test if we ran the HFS or NTFS
	 * specific tests, they would have already rejected `.git`.
	 */
	if ((flags & GIT_PATH_REJECT_DOT_GIT_HFS) == 0 &&
	    (flags & GIT_PATH_REJECT_DOT_GIT_NTFS) == 0 &&
	    (flags & GIT_PATH_REJECT_DOT_GIT_LITERAL)) {
		if (len >= 4 &&
		    component[0] == '.' &&
		    (component[1] == 'g' || component[1] == 'G') &&
		    (component[2] == 'i' || component[2] == 'I') &&
		    (component[3] == 't' || component[3] == 'T')) {
			if (len == 4)
				return false;

			if (S_ISLNK(mode) && common_prefix_icase(component, len, ".gitmodules") == len)
				return false;
		}
	    }

	return true;
}