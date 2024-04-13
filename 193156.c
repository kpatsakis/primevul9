int gitmodule_sha1_from_commit(const unsigned char *treeish_name,
				      unsigned char *gitmodules_sha1,
				      struct strbuf *rev)
{
	int ret = 0;

	if (is_null_sha1(treeish_name)) {
		hashclr(gitmodules_sha1);
		return 1;
	}

	strbuf_addf(rev, "%s:.gitmodules", sha1_to_hex(treeish_name));
	if (get_sha1(rev->buf, gitmodules_sha1) >= 0)
		ret = 1;

	return ret;
}