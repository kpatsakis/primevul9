set_up_outpe(context *ctx, int fd, Pe *inpe, Pe **outpe)
{
	size_t size;
	char *addr;

	addr = pe_rawfile(inpe, &size);

	off_t offset = lseek(fd, 0, SEEK_SET);
	if (offset < 0) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not read output file: %m");
		return -1;
	}

	int rc = ftruncate(fd, size);
	if (rc < 0) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not extend output file: %m");
		return -1;
	}
	rc = write(fd, addr, size);
	if (rc < 0) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not write to output file: %m");
		return -1;
	}

	*outpe = pe_begin(fd, PE_C_RDWR_MMAP, NULL);
	if (!*outpe)
		*outpe = pe_begin(fd, PE_C_RDWR, NULL);
	if (!*outpe) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not set up output: %s",
			pe_errmsg(pe_errno()));
		return -1;
	}

	pe_clearcert(*outpe);
	return 0;
}