int32_t cli_bcapi_buffer_pipe_new(struct cli_bc_ctx *ctx, uint32_t size)
{
    unsigned char *data;
    struct bc_buffer *b;
    unsigned n = ctx->nbuffers + 1;

    data = cli_calloc(1, size);
    if (!data)
	return -1;
    b = cli_realloc(ctx->buffers, sizeof(*ctx->buffers)*n);
    if (!b) {
	free(data);
	return -1;
    }
    ctx->buffers = b;
    ctx->nbuffers = n;
    b = &b[n-1];

    b->data = data;
    b->size = size;
    b->write_cursor = b->read_cursor = 0;
    return n-1;
}