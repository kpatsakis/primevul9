do_stat(int fd, size_t baselen, const char *path, struct stat *pst, int flags, rb_encoding *enc)
{
#if USE_OPENDIR_AT
    int ret = fstatat(fd, at_subpath(fd, baselen, path), pst, 0);
#else
    int ret = STAT(path, pst);
#endif
    if (ret < 0 && !to_be_ignored(errno))
	sys_warning(path, enc);

    return ret;
}