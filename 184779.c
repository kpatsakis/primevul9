static void update_common_dir(struct strbuf *buf, int git_dir_len,
			      const char *common_dir)
{
	char *base = buf->buf + git_dir_len;
	init_common_trie();
	if (!common_dir)
		common_dir = get_git_common_dir();
	if (trie_find(&common_trie, base, check_common, NULL) > 0)
		replace_dir(buf, git_dir_len, common_dir);
}