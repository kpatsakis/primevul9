int32_t cli_bcapi_hashset_add(struct cli_bc_ctx *ctx , int32_t id, uint32_t key)
{
    struct cli_hashset *s = get_hashset(ctx, id);
    if (!s)
	return -1;
    return cli_hashset_addkey(s, key);
}