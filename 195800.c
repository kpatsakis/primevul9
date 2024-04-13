int cli_bytecode_context_setfile(struct cli_bc_ctx *ctx, fmap_t *map)
{
    ctx->fmap = map;
    ctx->file_size = map->len + map->offset;
    ctx->hooks.filesize = &ctx->file_size;
    return 0;
}