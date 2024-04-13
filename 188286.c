bool git_path_is_empty_dir(const char *path)
{
	git_win32_path filter_w;
	bool empty = false;

	if (git_win32__findfirstfile_filter(filter_w, path)) {
		WIN32_FIND_DATAW findData;
		HANDLE hFind = FindFirstFileW(filter_w, &findData);

		/* FindFirstFile will fail if there are no children to the given
		 * path, which can happen if the given path is a file (and obviously
		 * has no children) or if the given path is an empty mount point.
		 * (Most directories have at least directory entries '.' and '..',
		 * but ridiculously another volume mounted in another drive letter's
		 * path space do not, and thus have nothing to enumerate.)  If
		 * FindFirstFile fails, check if this is a directory-like thing
		 * (a mount point).
		 */
		if (hFind == INVALID_HANDLE_VALUE)
			return git_path_isdir(path);

		/* If the find handle was created successfully, then it's a directory */
		empty = true;

		do {
			/* Allow the enumeration to return . and .. and still be considered
			 * empty. In the special case of drive roots (i.e. C:\) where . and
			 * .. do not occur, we can still consider the path to be an empty
			 * directory if there's nothing there. */
			if (!git_path_is_dot_or_dotdotW(findData.cFileName)) {
				empty = false;
				break;
			}
		} while (FindNextFileW(hFind, &findData));

		FindClose(hFind);
	}

	return empty;
}