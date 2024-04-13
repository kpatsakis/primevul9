int git_path_iconv_init_precompose(git_path_iconv_t *ic)
{
	git_buf_init(&ic->buf, 0);
	ic->map = iconv_open(GIT_PATH_REPO_ENCODING, GIT_PATH_NATIVE_ENCODING);
	return 0;
}