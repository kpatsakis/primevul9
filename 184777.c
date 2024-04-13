const char *worktree_git_path(const struct worktree *wt, const char *fmt, ...)
{
	struct strbuf *pathname = get_pathname();
	va_list args;
	va_start(args, fmt);
	do_git_path(wt, pathname, fmt, args);
	va_end(args);
	return pathname->buf;
}