static bool verify_dotgit_hfs_generic(const char *path, size_t len, const char *needle, size_t needle_len)
{
	size_t i;
	char c;

	if (next_hfs_char(&path, &len) != '.')
		return true;

	for (i = 0; i < needle_len; i++) {
		c = next_hfs_char(&path, &len);
		if (c != needle[i])
			return true;
	}

	if (next_hfs_char(&path, &len) != '\0')
		return true;

	return false;
}