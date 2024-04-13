int32_t cli_bcapi_map_remove(struct cli_bc_ctx *ctx , const uint8_t* key, int32_t keysize, int32_t id)
{
    struct cli_map *s = get_hashtab(ctx, id);
    if (!s)
	return -1;
    return cli_map_removekey(s, key, keysize);
}