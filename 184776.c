static void adjust_git_path(struct strbuf *buf, int git_dir_len)
{
	const char *base = buf->buf + git_dir_len;
	if (git_graft_env && is_dir_file(base, "info", "grafts"))
		strbuf_splice(buf, 0, buf->len,
			      get_graft_file(), strlen(get_graft_file()));
	else if (git_index_env && !strcmp(base, "index"))
		strbuf_splice(buf, 0, buf->len,
			      get_index_file(), strlen(get_index_file()));
	else if (git_db_env && dir_prefix(base, "objects"))
		replace_dir(buf, git_dir_len + 7, get_object_directory());
	else if (git_hooks_path && dir_prefix(base, "hooks"))
		replace_dir(buf, git_dir_len + 5, git_hooks_path);
	else if (git_common_dir_env)
		update_common_dir(buf, git_dir_len, NULL);
}