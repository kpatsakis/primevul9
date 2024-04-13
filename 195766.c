int32_t cli_bcapi_map_getvaluesize(struct cli_bc_ctx *ctx, int32_t id)
{
    struct cli_map *s = get_hashtab(ctx, id);
    if (!s)
	return -1;
    return cli_map_getvalue_size(s);
}