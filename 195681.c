int32_t cli_bcapi_map_addkey(struct cli_bc_ctx *ctx , const uint8_t* key, int32_t keysize, int32_t id)
{
    struct cli_map *s = get_hashtab(ctx, id);
    if (!s)
	return -1;
    return cli_map_addkey(s, key, keysize);
}