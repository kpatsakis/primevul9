uint32_t cli_bcapi_buffer_pipe_write_avail(struct cli_bc_ctx *ctx, int32_t id)
{
    struct bc_buffer *b = get_buffer(ctx, id);
    if (!b)
	return 0;
    if (!b->data)
	return 0;
    if (b->write_cursor >= b->size)
	return 0;
    return b->size - b->write_cursor;
}