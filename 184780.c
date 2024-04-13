void strbuf_git_path(struct strbuf *sb, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	do_git_path(NULL, sb, fmt, args);
	va_end(args);
}