char *git_pathdup(const char *fmt, ...)
{
	struct strbuf path = STRBUF_INIT;
	va_list args;
	va_start(args, fmt);
	do_git_path(NULL, &path, fmt, args);
	va_end(args);
	return strbuf_detach(&path, NULL);
}