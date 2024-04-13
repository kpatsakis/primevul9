uint8_t* cli_bcapi_map_getvalue(struct cli_bc_ctx *ctx , int32_t id, int32_t valuesize)
{
    struct cli_map *s = get_hashtab(ctx, id);
    if (!s)
	return NULL;
    if (cli_map_getvalue_size(s) != valuesize)
	return NULL;
    return cli_map_getvalue(s);
}