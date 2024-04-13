do_lstat(int fd, size_t baselen, const char *path, struct stat *pst, int flags, rb_encoding *enc)
{
#if USE_OPENDIR_AT
    int ret = fstatat(fd, at_subpath(fd, baselen, path), pst, AT_SYMLINK_NOFOLLOW);
#else
    int ret = lstat(path, pst);
#endif
    if (ret < 0 && !to_be_ignored(errno))
	sys_warning(path, enc);

    return ret;
}