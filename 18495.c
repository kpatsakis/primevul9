set_up_inpe(context *ctx, int fd, Pe **pe)
{
	*pe = pe_begin(fd, PE_C_READ_MMAP, NULL);
	if (!*pe)
		*pe = pe_begin(fd, PE_C_READ, NULL);
	if (!*pe) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not parse PE binary: %s",
			pe_errmsg(pe_errno()));
		return -1;
	}

	int rc = parse_signatures(&ctx->cms->signatures,
				  &ctx->cms->num_signatures, *pe);
	if (rc < 0) {
		ctx->cms->log(ctx->cms, ctx->priority|LOG_ERR,
			"could not parse signature list");
		pe_end(*pe);
		*pe = NULL;
		return -1;
	}
	return 0;
}