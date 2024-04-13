int32_t cli_bcapi_buffer_pipe_done(struct cli_bc_ctx *ctx , int32_t id)
{
    struct bc_buffer *b = get_buffer(ctx, id);
    if (!b)
	return -1;
    free(b->data);
    b->data = NULL;
    return -0;
}