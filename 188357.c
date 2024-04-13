bool git_path_supports_symlinks(const char *dir)
{
	git_buf path = GIT_BUF_INIT;
	bool supported = false;
	struct stat st;
	int fd;

	if ((fd = git_futils_mktmp(&path, dir, 0666)) < 0 ||
	    p_close(fd) < 0 ||
	    p_unlink(path.ptr) < 0 ||
	    p_symlink("testing", path.ptr) < 0 ||
	    p_lstat(path.ptr, &st) < 0)
		goto done;

	supported = (S_ISLNK(st.st_mode) != 0);
done:
	if (path.size)
		(void)p_unlink(path.ptr);
	git_buf_dispose(&path);
	return supported;
}