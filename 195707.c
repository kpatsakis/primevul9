int32_t cli_bcapi_hashset_new(struct cli_bc_ctx *ctx )
{
    unsigned  n = ctx->nhashsets+1;
    struct cli_hashset *s = cli_realloc(ctx->hashsets, sizeof(*ctx->hashsets)*n);
    if (!s) {
	cli_event_error_oom(EV, 0);
	return -1;
    }
    ctx->hashsets = s;
    ctx->nhashsets = n;
    s = &s[n-1];
    cli_hashset_init(s, 16, 80);
    return n-1;
}