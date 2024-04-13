add_token_to_authenticated_list(context *ctx, uint8_t *tokenname)
{
	char *tmp;
	uint8_t **newtokennames = realloc(ctx->tokennames,
					sizeof (uint8_t *)
					* (ctx->ntokennames+1));
	if (!newtokennames)
		return -1;
	ctx->tokennames = newtokennames;

	tmp = strdup((char *)tokenname);
	if (!tmp)
		return -1;

	newtokennames[ctx->ntokennames++] = (uint8_t *)tmp;

	qsort(newtokennames, ctx->ntokennames, sizeof (char *), cmpstringp);
	return 0;
}