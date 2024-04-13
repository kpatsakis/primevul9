void report_linked_checkout_garbage(void)
{
	struct strbuf sb = STRBUF_INIT;
	const struct common_dir *p;
	int len;

	if (!git_common_dir_env)
		return;
	strbuf_addf(&sb, "%s/", get_git_dir());
	len = sb.len;
	for (p = common_list; p->dirname; p++) {
		const char *path = p->dirname;
		if (p->ignore_garbage)
			continue;
		strbuf_setlen(&sb, len);
		strbuf_addstr(&sb, path);
		if (file_exists(sb.buf))
			report_garbage(PACKDIR_FILE_GARBAGE, sb.buf);
	}
	strbuf_release(&sb);
}