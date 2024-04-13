static void context_safe(struct cli_bc_ctx *ctx)
{
    /* make sure these are never NULL */
    if (!ctx->hooks.kind)
	ctx->hooks.kind = &nokind;
    if (!ctx->hooks.match_counts)
	ctx->hooks.match_counts = nomatch;
    if (!ctx->hooks.match_offsets)
	ctx->hooks.match_counts = nooffsets;
    if (!ctx->hooks.filesize)
	ctx->hooks.filesize = &nofilesize;
    if (!ctx->hooks.pedata)
	ctx->hooks.pedata = &nopedata;
}