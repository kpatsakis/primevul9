    is_subdir(const char *subdir, const char *dir)
{
	size_t dirlen = strlen(dir);
	size_t subdirlen = strlen(subdir);

	// @dir has to be at least as long as @subdir
	if (subdirlen < dirlen)
		return false;
	// @dir has to be a prefix of @subdir
	if (strncmp(subdir, dir, dirlen) != 0)
		return false;
	// @dir can look like "path/" (that is, end with the directory separator).
	// When that is the case then given the test above we can be sure @subdir
	// is a real subdirectory.
	if (dirlen > 0 && dir[dirlen - 1] == '/')
		return true;
	// @subdir can look like "path/stuff" and when the directory separator
	// is exactly at the spot where @dir ends (that is, it was not caught
	// by the test above) then @subdir is a real subdirectory.
	if (subdir[dirlen] == '/' && dirlen > 0)
		return true;
	// If both @dir and @subdir have identical length then given that the
	// prefix check above @subdir is a real subdirectory.
	if (subdirlen == dirlen)
		return true;
	return false;
}