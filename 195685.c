int32_t cli_bcapi_map_setvalue(struct cli_bc_ctx *ctx, const uint8_t* value, int32_t valuesize, int32_t id)
{
    struct cli_map *s = get_hashtab(ctx, id);
    if (!s)
	return -1;
    return cli_map_setvalue(s, value, valuesize);
}