static int _pam_winbind_free_context(struct pwb_context *ctx)
{
	if (!ctx) {
		return 0;
	}

	if (ctx->dict) {
		iniparser_freedict(ctx->dict);
	}

	return 0;
}