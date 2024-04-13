const char *git_path(const char *fmt, ...)
{
	struct strbuf *pathname = get_pathname();
	va_list args;
	va_start(args, fmt);
	do_git_path(NULL, pathname, fmt, args);
	va_end(args);
	return pathname->buf;
}