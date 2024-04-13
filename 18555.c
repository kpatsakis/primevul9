sign_kmod(context *ctx, int infd, int outfd, int attached)
{
	unsigned char *map;
	struct stat statbuf;
	ssize_t sig_len;
	int rc;

	rc = fstat(infd, &statbuf);
	if (rc != 0) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not stat input file: %m");
		return rc;
	}

	map = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, infd, 0);
	if (map == MAP_FAILED) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not map input file: %m");
		return -1;

	}

	rc = kmod_generate_digest(ctx->cms, map, statbuf.st_size);
	if (rc < 0)
		goto out;

	if (attached) {
		rc = write_file(outfd, map, statbuf.st_size);
		if (rc) {
			ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
				"could not write module data: %m");
			goto out;
		}
	}

	sig_len = kmod_write_signature(ctx->cms, outfd);
	if (sig_len < 0) {
		rc = sig_len;
		goto out;
	}

	rc = kmod_write_sig_info(ctx->cms, outfd, sig_len);

out:
	munmap(map, statbuf.st_size);
	return rc;
}