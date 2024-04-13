uint8_t* cli_bcapi_buffer_pipe_write_get(struct cli_bc_ctx *ctx, int32_t id, uint32_t size)
{
    struct bc_buffer *b = get_buffer(ctx, id);
    if (!b || size > cli_bcapi_buffer_pipe_write_avail(ctx, id) || !size)
	return NULL;
    if (!b->data)
	return NULL;
    return b->data + b->write_cursor;
}