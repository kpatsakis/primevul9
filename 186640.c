cli_ppt_vba_read(int ifd, cli_ctx *ctx)
{
	char *dir;
	const char *ret;

	/* Create a directory to store the extracted OLE2 objects */
	dir = cli_gentemp(ctx ? ctx->engine->tmpdir : NULL);
	if(dir == NULL)
		return NULL;
	if(mkdir(dir, 0700)) {
		cli_errmsg("cli_ppt_vba_read: Can't create temporary directory %s\n", dir);
		free(dir);
		return NULL;
	}
	ret = ppt_stream_iter(ifd, dir);
	if(ret == NULL) {
		cli_rmdirs(dir);
		free(dir);
		return NULL;
	}
	return dir;
}