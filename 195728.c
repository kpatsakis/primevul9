uint32_t cli_bcapi_buffer_pipe_read_avail(struct cli_bc_ctx *ctx , int32_t id)
{
    struct bc_buffer *b = get_buffer(ctx, id);
    if (!b)
	return 0;
    if (b->data) {
	if (b->write_cursor <= b->read_cursor)
	    return 0;
	return b->write_cursor - b->read_cursor;
    }
    if (!ctx->fmap || b->read_cursor >= ctx->file_size)
	return 0;
    if (b->read_cursor + BUFSIZ <= ctx->file_size)
	return BUFSIZ;
    return ctx->file_size - b->read_cursor;
}