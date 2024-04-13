cli_scan_ole10(int fd, cli_ctx *ctx)
{
	int ofd, ret;
	uint32_t object_size;
	struct stat statbuf;
	char *fullname;

	if(fd < 0)
		return CL_CLEAN;

	lseek(fd, 0, SEEK_SET);
	if(!read_uint32(fd, &object_size, FALSE))
		return CL_CLEAN;

	if(fstat(fd, &statbuf) == -1)
		return CL_ESTAT;

	if ((statbuf.st_size - object_size) >= 4) {
		/* Probably the OLE type id */
		if (lseek(fd, 2, SEEK_CUR) == -1) {
			return CL_CLEAN;
		}

		/* Attachment name */
		if(!skip_past_nul(fd))
			return CL_CLEAN;

		/* Attachment full path */
		if(!skip_past_nul(fd))
			return CL_CLEAN;

		/* ??? */
		if(lseek(fd, 8, SEEK_CUR) == -1)
			return CL_CLEAN;

		/* Attachment full path */
		if(!skip_past_nul(fd))
			return CL_CLEAN;

		if(!read_uint32(fd, &object_size, FALSE))
			return CL_CLEAN;
	}
	if(!(fullname = cli_gentemp(ctx ? ctx->engine->tmpdir : NULL))) {
		return CL_EMEM;
	}
	ofd = open(fullname, O_RDWR|O_CREAT|O_TRUNC|O_BINARY|O_EXCL,
		S_IWUSR|S_IRUSR);
	if (ofd < 0) {
		cli_warnmsg("cli_decode_ole_object: can't create %s\n",	fullname);
		free(fullname);
		return CL_ECREAT;
	}
	cli_dbgmsg("cli_decode_ole_object: decoding to %s\n", fullname);
	ole_copy_file_data(fd, ofd, object_size);
	lseek(ofd, 0, SEEK_SET);
	ret = cli_magic_scandesc(ofd, ctx);
	close(ofd);
	if(ctx && !ctx->engine->keeptmp)
	  if (cli_unlink(fullname))
	    ret = CL_EUNLINK;
	free(fullname);
	return ret;
}