char *git_path_buf(struct strbuf *buf, const char *fmt, ...)
{
	va_list args;
	strbuf_reset(buf);
	va_start(args, fmt);
	do_git_path(NULL, buf, fmt, args);
	va_end(args);
	return buf->buf;
}