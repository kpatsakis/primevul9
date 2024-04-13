int32_t cli_bcapi_input_switch(struct cli_bc_ctx *ctx , int32_t extracted_file)
{
    fmap_t *map;
    if (ctx->extracted_file_input == extracted_file)
	return 0;
    if (!extracted_file) {
	cli_dbgmsg("bytecode api: input switched back to main file\n");
	ctx->fmap = ctx->save_map;
	ctx->extracted_file_input = 0;
	return 0;
    }
    if (ctx->outfd < 0)
	return -1;
    map = fmap(ctx->outfd, 0, 0);
    if (!map) {
	cli_warnmsg("can't mmap() extracted temporary file %s\n", ctx->tempfile);
	return -1;
    }
    ctx->save_map = ctx->fmap;
    cli_bytecode_context_setfile(ctx, map);
    ctx->extracted_file_input = 1;
    cli_dbgmsg("bytecode api: input switched to extracted file\n");
    return 0;
}