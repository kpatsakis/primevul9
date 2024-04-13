int32_t cli_bcapi_buffer_pipe_new_fromfile(struct cli_bc_ctx *ctx , uint32_t at)
{
    struct bc_buffer *b;
    unsigned n = ctx->nbuffers + 1;

    if (at >= ctx->file_size)
	return -1;

    b = cli_realloc(ctx->buffers, sizeof(*ctx->buffers)*n);
    if (!b) {
	return -1;
    }
    ctx->buffers = b;
    ctx->nbuffers = n;
    b = &b[n-1];

    /* NULL data means read from file at pos read_cursor */
    b->data = NULL;
    b->size = 0;
    b->read_cursor = at;
    b->write_cursor = 0;
    return n-1;
}