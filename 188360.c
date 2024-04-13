void git_path_iconv_clear(git_path_iconv_t *ic)
{
	if (ic) {
		if (ic->map != (iconv_t)-1)
			iconv_close(ic->map);
		git_buf_dispose(&ic->buf);
	}
}