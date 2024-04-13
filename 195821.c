int32_t cli_bcapi_buffer_pipe_write_stopped(struct cli_bc_ctx *ctx , int32_t id, uint32_t size)
{
    struct bc_buffer *b = get_buffer(ctx, id);
    if (!b || !b->data)
	return -1;
    if (b->write_cursor + size >= b->size)
	b->write_cursor = b->size;
    else
	b->write_cursor += size;
    return 0;
}