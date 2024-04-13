int32_t cli_bcapi_buffer_pipe_read_stopped(struct cli_bc_ctx *ctx , int32_t id, uint32_t amount)
{
    struct bc_buffer *b = get_buffer(ctx, id);
    if (!b)
	return -1;
    if (b->data) {
	if (b->write_cursor <= b->read_cursor)
	    return -1;
	if (b->read_cursor + amount > b->write_cursor)
	    b->read_cursor = b->write_cursor;
	else
	    b->read_cursor += amount;
	if (b->read_cursor >= b->size &&
	    b->write_cursor >= b->size)
	    b->read_cursor = b->write_cursor = 0;
	return 0;
    }
    b->read_cursor += amount;
    return 0;
}