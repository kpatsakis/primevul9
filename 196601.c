at_subpath(int fd, size_t baselen, const char *path)
{
#if USE_OPENDIR_AT
    if (fd != (int)AT_FDCWD && baselen > 0) {
	path += baselen;
	if (*path == '/') ++path;
    }
#endif
    return *path ? path : ".";
}