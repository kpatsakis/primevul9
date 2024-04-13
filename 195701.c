static struct bc_buffer *get_buffer(struct cli_bc_ctx *ctx, int32_t id)
{
    if (!ctx->buffers || id < 0 || id >= ctx->nbuffers) {
	cli_dbgmsg("bytecode api: invalid buffer id %u\n", id);
	return NULL;
    }
    return &ctx->buffers[id];
}