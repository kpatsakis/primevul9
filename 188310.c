void git_path_diriter_free(git_path_diriter *diriter)
{
	if (diriter == NULL)
		return;

	if (diriter->dir) {
		closedir(diriter->dir);
		diriter->dir = NULL;
	}

#ifdef GIT_USE_ICONV
	git_path_iconv_clear(&diriter->ic);
#endif

	git_buf_dispose(&diriter->path);
}