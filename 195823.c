uint8_t* cli_bcapi_buffer_pipe_read_get(struct cli_bc_ctx *ctx , int32_t id, uint32_t size)
{
    struct bc_buffer *b = get_buffer(ctx, id);
    if (!b || size > cli_bcapi_buffer_pipe_read_avail(ctx, id) || !size)
	return NULL;
    if (b->data)
	return b->data + b->read_cursor;
    return fmap_need_off(ctx->fmap, b->read_cursor, size);
}