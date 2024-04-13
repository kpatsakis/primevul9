int32_t cli_bcapi_file_find(struct cli_bc_ctx *ctx, const uint8_t* data, uint32_t len)
{
    fmap_t *map = ctx->fmap;
    if (!map || len <= 0) {
	cli_dbgmsg("bcapi_file_find preconditions not met\n");
	API_MISUSE();
	return -1;
    }
    return cli_bcapi_file_find_limit(ctx, data, len, map->len);
}