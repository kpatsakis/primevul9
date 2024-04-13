unreadable_info(struct magic_set *ms, mode_t md, const char *file)
{
	/* We cannot open it, but we were able to stat it. */
	if (access(file, W_OK) == 0)
		if (file_printf(ms, "writable, ") == -1)
			return -1;
	if (access(file, X_OK) == 0)
		if (file_printf(ms, "executable, ") == -1)
			return -1;
	if (S_ISREG(md))
		if (file_printf(ms, "regular file, ") == -1)
			return -1;
	if (file_printf(ms, "no read permission") == -1)
		return -1;
	return 0;
}