static void do_git_common_path(struct strbuf *buf,
			       const char *fmt,
			       va_list args)
{
	strbuf_addstr(buf, get_git_common_dir());
	if (buf->len && !is_dir_sep(buf->buf[buf->len - 1]))
		strbuf_addch(buf, '/');
	strbuf_vaddf(buf, fmt, args);
	strbuf_cleanup_path(buf);
}