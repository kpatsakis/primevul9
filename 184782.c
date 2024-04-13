static void do_git_path(const struct worktree *wt, struct strbuf *buf,
			const char *fmt, va_list args)
{
	int gitdir_len;
	strbuf_addstr(buf, get_worktree_git_dir(wt));
	if (buf->len && !is_dir_sep(buf->buf[buf->len - 1]))
		strbuf_addch(buf, '/');
	gitdir_len = buf->len;
	strbuf_vaddf(buf, fmt, args);
	adjust_git_path(buf, gitdir_len);
	strbuf_cleanup_path(buf);
}