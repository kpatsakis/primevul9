GIT_INLINE(bool) verify_dotgit_ntfs_generic(const char *name, size_t len, const char *dotgit_name, size_t dotgit_len, const char *shortname_pfix)
{
	int i, saw_tilde;

	if (name[0] == '.' && len >= dotgit_len &&
	    !strncasecmp(name + 1, dotgit_name, dotgit_len)) {
		return !only_spaces_and_dots(name + dotgit_len + 1);
	}

	/* Detect the basic NTFS shortname with the first six chars */
	if (!strncasecmp(name, dotgit_name, 6) && name[6] == '~' &&
	    name[7] >= '1' && name[7] <= '4')
		return !only_spaces_and_dots(name + 8);

	/* Catch fallback names */
	for (i = 0, saw_tilde = 0; i < 8; i++) {
		if (name[i] == '\0') {
			return true;
		} else if (saw_tilde) {
			if (name[i] < '0' || name[i] > '9')
				return true;
		} else if (name[i] == '~') {
			if (name[i+1] < '1' || name[i+1]  > '9')
				return true;
			saw_tilde = 1;
		} else if (i >= 6) {
			return true;
		} else if ((unsigned char)name[i] > 127) {
			return true;
		} else if (git__tolower(name[i]) != shortname_pfix[i]) {
			return true;
		}
	}

	return !only_spaces_and_dots(name + i);
}